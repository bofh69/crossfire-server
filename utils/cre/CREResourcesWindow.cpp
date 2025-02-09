/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include <Qt>
#include <QtWidgets>
#include <QScriptValue>
#include <stdexcept>

#include "CREResourcesWindow.h"
#include "CREPixmap.h"

#include "CREFilterDialog.h"
#include "CREFilterDefinition.h"

#include "CRESettings.h"

#include "CREReportDialog.h"
#include "CREReportDisplay.h"
#include "CREReportDefinition.h"

#include "assets/AssetWrapperPanel.h"
#include "animations/AnimationPanel.h"
#include "archetypes/ArchetypePanel.h"
#include "treasures/TreasureListPanel.h"
#include "treasures/TreasurePanel.h"
#include "artifacts/ArtifactListPanel.h"
#include "artifacts/ArtifactPanel.h"
#include "recipes/RecipePanel.h"
#include "CREMapPanel.h"
#include "regions/RegionPanel.h"
#include "CREQuestPanel.h"
#include "CREMessagePanel.h"
#include "scripts/ScriptFilePanel.h"
#include "general_messages/GeneralMessagePanel.h"
#include "faces/FacesetsPanel.h"
#include "attack_messages/AttackMessagePanel.h"

#include "artifacts/ArtifactWrapper.h"

#include "CREMapInformationManager.h"
#include "MessageFile.h"
#include "scripts/ScriptFileManager.h"

#include "CREScriptEngine.h"

#include "random_maps/RandomMap.h"
#include "random_maps/RandomMapPanel.h"

#include "global.h"
#include "recipe.h"
#include "assets.h"
#include "AssetsManager.h"

#include "MessageManager.h"
#include "ResourcesManager.h"
#include "assets/AssetModel.h"
#include "faces/FacePanel.h"
#include "sounds/SoundFilesPanel.h"
#include "sounds/SoundFilePanel.h"
#include "sounds/GameSoundsPanel.h"
#include "sounds/GameSoundPanel.h"
#include "QuickFilterDialog.h"

CREResourcesWindow::CREResourcesWindow(CREMapInformationManager* store, MessageManager* messages, ResourcesManager* resources, ScriptFileManager* scripts, AssetModel *model, const QModelIndex &root, QWidget* parent) : QWidget(parent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setWindowTitle(model->data(root, Qt::DisplayRole).toString());

    Q_ASSERT(store);
    myStore = store;
    Q_ASSERT(messages);
    myMessages = messages;
    Q_ASSERT(resources);
    myResources = resources;
    Q_ASSERT(scripts);
    myScripts = scripts;
    myModel = new ScriptFilterAssetModel(model, &myEngine, this);
    myTreeRoot = root;

    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* layout = new QVBoxLayout(this);

    myFiltersMenu = new QMenu(this);
    QHBoxLayout* buttons = new QHBoxLayout();
    myFilterButton = new QPushButton(tr("Filter..."), this);
    myFilterButton->setMenu(myFiltersMenu);
    buttons->addWidget(myFilterButton);

    myReportsMenu = new QMenu(this);
    QPushButton* report = new QPushButton(tr("Report"), this);
    report->setMenu(myReportsMenu);
    buttons->addWidget(report);

    auto exportCsv = new QPushButton(tr("Export as CSV"), this);
    buttons->addWidget(exportCsv);
    connect(exportCsv, SIGNAL(clicked()), this, SLOT(onExportAsCsv()));
    if (!root.isValid() || !static_cast<const AssetWrapper *>(root.internalPointer())->canExportAsCsv())
    {
        exportCsv->setEnabled(false);
        exportCsv->setToolTip(tr("CSV export is not available for this type of ressources"));
    }
    else
    {
        exportCsv->setToolTip(tr("Export visible ressources to CSV file"));
    }

    layout->addLayout(buttons);

    auto splitter = new QSplitter(this);
    layout->addWidget(splitter);

    myTree = new QTreeView(this);
    myTree->setModel(myModel);
    myTree->setRootIndex(myModel->mapFromSource(root));
    splitter->addWidget(myTree);
    myTree->setIconSize(QSize(32, 32));
    myTree->collapseAll();
    myTree->expand(myModel->mapFromSource(root));
    myTree->setSelectionMode(QAbstractItemView::SingleSelection);
    myTree->setDragEnabled(true);
    myTree->setDropIndicatorShown(true);
    myTree->setDragDropMode(QAbstractItemView::DragDrop);
    myTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(treeCustomMenu(const QPoint&)));
    connect(myTree->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentRowChanged(const QModelIndex&, const QModelIndex&)));
    connect(myModel, &QAbstractItemModel::rowsInserted, [this](const QModelIndex &parent, int /*first*/, int /*last*/) {
        myTree->expand(parent);
    });

    QWidget* w = new QWidget(splitter);
    myStackedPanels = new QStackedLayout(w);
    splitter->addWidget(w);

    /* dummy panel to display for empty items */
    AssetWrapperPanel* dummy = new AssetWrapperPanel(this);
    dummy->addLabel(tr("No details available."), nullptr);
    addPanel("empty", dummy);
    myStackedPanels->setCurrentWidget(dummy);
    myCurrentPanel = dummy;

    connect(&myFiltersMapper, SIGNAL(mapped(QObject*)), this, SLOT(onFilterChange(QObject*)));
    updateFilters();
    connect(&myReportsMapper, SIGNAL(mapped(QObject*)), this, SLOT(onReportChange(QObject*)));
    updateReports();

    addPanel("Archetype", new ArchetypePanel(model, this));
    addPanel("Face", new CREFacePanel(this, model, myResources, myStore));
    addPanel("Animation", new AnimationPanel(this, model));
    addPanel("Artifact", new ArtifactPanel(this, myResources));
    addPanel("ArtifactList", new ArtifactListPanel(this));
    addPanel("Recipe", new RecipePanel(this));
    addPanel("Treasure", new TreasurePanel(this));
    addPanel("TreasureList", new CRETreasurePanel(model, this));
    addPanel("Faceset", new FacesetsPanel(this, myResources->licenseManager()));
    addPanel("Quest", new CREQuestPanel(model, this));
    addPanel("QuestStep", new QuestStepPanel(myMessages, this));
    addPanel("GeneralMessage", new CREGeneralMessagePanel(this));
    addPanel("Region", new RegionPanel(this));
    addPanel("Map", new CREMapPanel(myScripts, this));
    addPanel("Script", new CREScriptPanel(model, this));
    addPanel("Message", new CREMessagePanel(myMessages, this));
    addPanel("RandomMap", new CRERandomMapPanel(this));
    addPanel("AttackMessage", new AttackMessagePanel(this));
    addPanel("SoundFiles", new SoundFilesPanel(this));
    addPanel("SoundFile", new SoundFilePanel(this, model));
    addPanel("GameSounds", new GameSoundsPanel(this));
    addPanel("GameSound", new GameSoundPanel(this));

    splitter->setSizes({5000, 5000});

    QApplication::restoreOverrideCursor();
}

CREResourcesWindow::~CREResourcesWindow()
{
    qDeleteAll(myPanels);
}

void CREResourcesWindow::currentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    if (!current.isValid()) {
        myCurrentPanel = nullptr;
        return;
    }

    auto rc = myModel->mapToSource(current);
    AssetWrapper *item = reinterpret_cast<AssetWrapper *>(rc.internalPointer());
    if (!item) {
        return;
    }

    auto newPanel = myPanels[item->displayPanelName()];
    if (!newPanel) {
//        printf("no panel for %s\n", qPrintable(item->getPanelName()));
        return;
    }

    newPanel->setAsset(item);

    if (myCurrentPanel != newPanel) {
        myStackedPanels->setCurrentWidget(newPanel);
        myCurrentPanel = newPanel;
    }
}

void CREResourcesWindow::addPanel(QString name, AssetWrapperPanel* panel)
{
    panel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    myPanels[name] = panel;
    myStackedPanels->addWidget(panel);
}

void CREResourcesWindow::onFilter()
{
    CREFilterDialog dlg;
    if (dlg.exec() != QDialog::Accepted)
        return;

    /* sending this signal will ultimately call our own updateFilters() */
    emit filtersModified();
}

void CREResourcesWindow::onReport()
{
    CREReportDialog dlg;
    if (dlg.exec() != QDialog::Accepted)
        return;

    /* sending this signal will ultimately call our own updateReports() */
    emit reportsModified();
}

void CREResourcesWindow::updateFilters()
{
    CRESettings settings;
    settings.loadFilters(myFilters);

    myFiltersMenu->clear();

    QAction* clear = new QAction(tr("(none)"), this);
    connect(clear, SIGNAL(triggered()), this, SLOT(clearFilter()));
    myFiltersMenu->addAction(clear);

    if (myFilters.filters().size() > 0)
    {

        foreach(CREFilterDefinition* filter, myFilters.filters())
        {
            QAction* a = new QAction(filter->name(), this);
            myFiltersMenu->addAction(a);
            myFiltersMapper.setMapping(a, filter);
            connect(a, SIGNAL(triggered()), &myFiltersMapper, SLOT(map()));
        }
    }
    myFiltersMenu->addSeparator();

    QAction* quick = new QAction(tr("Quick filter..."), this);
    connect(quick, SIGNAL(triggered()), this, SLOT(onQuickFilter()));
    myFiltersMenu->addAction(quick);
    QAction* dialog = new QAction(tr("Filters definition..."), this);
    connect(dialog, SIGNAL(triggered()), this, SLOT(onFilter()));
    myFiltersMenu->addAction(dialog);

    clearFilter();
}

void CREResourcesWindow::onFilterChange(QObject* object) {
    CREFilterDefinition* filter = qobject_cast<CREFilterDefinition*>(object);
    if (filter == NULL)
        return;
    setFilter(filter->filter(), filter->name());
}

void CREResourcesWindow::onQuickFilter() {
    QuickFilterDialog dlg(this, myModel->filter());
    if (dlg.exec() != QDialog::Accepted) {
      return;
    }
    setFilter(dlg.filter(), dlg.filter());
}

void CREResourcesWindow::clearFilter() {
    setFilter(QString(), QString());
}

void CREResourcesWindow::setFilter(const QString &filter, const QString &name) {
    myModel->setFilter(filter);
    myFilterButton->setText(filter.isEmpty() ? tr("Filter...") : tr("Filter: %1").arg(name));
    auto root = myModel->mapFromSource(myTreeRoot);
    if (!myTreeRoot.isValid() || root.isValid()) {
        if (myTree->model() == nullptr) {
            myTree->setModel(myModel);
            connect(myTree->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentRowChanged(const QModelIndex&, const QModelIndex&)));
        }
        myTree->setRootIndex(root);
    } else {
        myTree->setModel(nullptr);
    }
}

void CREResourcesWindow::updateReports()
{
    CRESettings settings;
    settings.loadReports(myReports);

    myReportsMenu->clear();

    if (myReports.reports().size() > 0)
    {
        foreach(CREReportDefinition* report, myReports.reports())
        {
            QAction* a = new QAction(report->name(), this);
            myReportsMenu->addAction(a);
            myReportsMapper.setMapping(a, report);
            connect(a, SIGNAL(triggered()), &myReportsMapper, SLOT(map()));
        }

        myReportsMenu->addSeparator();
    }

    QAction* dialog = new QAction(tr("Reports definition..."), this);
    connect(dialog, SIGNAL(triggered()), this, SLOT(onReport()));
    myReportsMenu->addAction(dialog);
}

void CREResourcesWindow::onReportChange(QObject* object)
{
    CREReportDefinition* report = qobject_cast<CREReportDefinition*>(object);
    if (report == NULL)
        return;

    int count = myModel->rowCount(myTree->rootIndex());

    QProgressDialog progress(tr("Generating report..."), tr("Abort report"), 0, count - 1, this);
    progress.setWindowTitle(tr("Report: '%1'").arg(report->name()));
    progress.setWindowModality(Qt::WindowModal);

    QStringList headers = report->header().split("\n");
    QStringList fields = report->itemDisplay().split("\n");
    QString sort = report->itemSort();

    QString text("<table><thead><tr>");

    foreach(QString header, headers)
    {
        text += "<th>" + header + "</th>";
    }
    text += "</tr></thead><tbody>";

    CREScriptEngine engine;
    std::vector<QScriptValue> items;
    for (int i = 0; i < count; i++) {
        auto idx = myModel->index(i, 0, myTree->rootIndex());
        if (!idx.isValid()) {
            continue;
        }
        idx = myModel->mapToSource(idx);
        auto w = static_cast<AssetWrapper *>(idx.internalPointer());
        items.push_back(engine.newQObject(w));
    }

    if (!sort.isEmpty())
    {
        try
        {
            progress.setLabelText(tr("Sorting items..."));

            engine.pushContext();

            sort = "(function(left, right) { return " + sort + "; })";
            QScriptValue sortFun = engine.evaluate(sort);
            if (!sortFun.isValid() || engine.hasUncaughtException())
                throw std::runtime_error("A script error happened while compiling the sort criteria:\n" + engine.uncaughtException().toString().toStdString());

            std::sort(items.begin(), items.end(), [&sortFun, &engine](QScriptValue left, QScriptValue right) {
                QScriptValueList args;
                args.push_back(left);
                args.push_back(right);
                auto ret = sortFun.call(QScriptValue(), args);
                if (!ret.isValid() || engine.hasUncaughtException())
                {
                    throw std::runtime_error("A script error happened while sorting items:\n" + engine.uncaughtException().toString().toStdString());
                    return false;
                }
                return ret.isValid() ? ret.toBoolean() : true;
            });
            printf("complete");
            engine.popContext();
        }
        catch (std::runtime_error& ex)
        {
            QMessageBox::critical(this, "Script error", ex.what(), QMessageBox::Ok);
            return;
        }
    }

    progress.setLabelText(tr("Generating items text..."));
    foreach(QScriptValue item, items)
    {
        if (progress.wasCanceled())
            return;

        text += "<tr>";

        engine.pushContext();
        engine.globalObject().setProperty("item", item);

        foreach(QString field, fields)
        {
            text += "<td>";
            QString data = engine.evaluate(field).toString();
            if (engine.hasUncaughtException())
            {
                QMessageBox::critical(this, "Script error", "A script error happened while display items:\n" + engine.uncaughtException().toString(), QMessageBox::Ok);
                return;
            }
            text += data;
            text += "</td>\n";
        }
        engine.popContext();
        text += "</tr>\n";

        progress.setValue(progress.value() + 1);
    }
    text += "</tbody>";

    QStringList footers = report->footer().split("\n");
    text += "<tfoot>";

    foreach(QString footer, footers)
    {
        text += "<th>" + footer + "</th>";
    }
    text += "</tfoot>";

    text += "</table>";
    qDebug() << "report finished";

    CREReportDisplay display(text, tr("Report: '%1'").arg(report->name()));
    display.exec();
    progress.hide();
}

void CREResourcesWindow::onExportAsCsv()
{
    assert(myTreeRoot.isValid());
    auto root = static_cast<const AssetWrapper *>(myTreeRoot.internalPointer());
    assert(root->canExportAsCsv());

    auto destination = QFileDialog::getSaveFileName(this, tr("Export as CSV..."), QString(), tr("CSV file (*.csv);;All files (*.*)"));
    if (destination.isEmpty())
        return;

    QString contents;
    root->fillCsvHeader(contents);

    int count = myModel->rowCount(myTree->rootIndex());

    for (int i = 0; i < count; i++) {
        auto idx = myModel->index(i, 0, myTree->rootIndex());
        if (!idx.isValid()) {
            continue;
        }
        idx = myModel->mapToSource(idx);
        auto w = static_cast<const AssetWrapper *>(idx.internalPointer());
        root->exportAsCSV(w, contents);
    }

    QFile file(destination);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(this, tr("Error writing CSV file %1").arg(destination), tr("Unable to write CSV file %1!").arg(destination));
        return;
    }

    file.write(contents.toLocal8Bit());
    QMessageBox::information(this, tr("Export complete"), tr("Ressources correctly exported to CSV file %1.").arg(destination));
}

void CREResourcesWindow::treeCustomMenu(const QPoint & pos)
{
    QMenu menu;

    QModelIndex index = myModel->mapToSource(myTree->indexAt(pos));
    if (index.isValid() && index.internalPointer()) {
        AssetWrapper *item = reinterpret_cast<AssetWrapper *>(index.internalPointer());
        if (item) {
            item->fillMenu(&menu);
        }
    }

    if (menu.actions().size() == 0)
        return;
    menu.exec(myTree->mapToGlobal(pos) + QPoint(5, 5));
}

void CREResourcesWindow::addQuest(bool)
{
    auto name = QInputDialog::getText(this, "Create new quest", "New quest code").toStdString();
    if (name.empty()) {
        return;
    }
    if (getManager()->quests()->find(name)) {
        QMessageBox::critical(this, "Quest already exists", tr("Quest %1 already exists!").arg(name.data()));
        return;
    }

    auto quest = quest_create(name.data());
    quest->face = getManager()->faces()->get("quest_generic.111");
    getManager()->quests()->define(name, quest);
}

void CREResourcesWindow::addMessage(bool)
{
#if 0
    MessageFile* file = new MessageFile("<new file>");
    file->setModified();
    myMessages->messages().append(file);
#endif
}
