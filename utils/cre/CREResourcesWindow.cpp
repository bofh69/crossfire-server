#include <Qt>
#include <QtWidgets>
#include <QScriptValue>
#include <stdexcept>

#include "CREResourcesWindow.h"
#include "CREUtils.h"
#include "CREPixmap.h"

#include "CREFilterDialog.h"
#include "CREFilterDefinition.h"

#include "CRESettings.h"

#include "CREReportDialog.h"
#include "CREReportDisplay.h"
#include "CREReportDefinition.h"

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

#include "artifacts/ArtifactWrapper.h"

#include "CREMapInformationManager.h"
#include "MessageFile.h"
#include "scripts/ScriptFileManager.h"

#include "CREScriptEngine.h"

#include "random_maps/RandomMap.h"
#include "random_maps/RandomMapPanel.h"

extern "C" {
#include "global.h"
#include "recipe.h"
}
#include "assets.h"
#include "AssetsManager.h"

#include "MessageManager.h"
#include "ResourcesManager.h"
#include "assets/AssetModel.h"
#include "faces/FacePanel.h"

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
    CREPanel* dummy = new CREPanel(this);
    QVBoxLayout* dl = new QVBoxLayout(dummy);
    dl->addWidget(new QLabel(tr("No details available."), dummy));
    addPanel("empty", dummy);
    myStackedPanels->setCurrentWidget(dummy);
    myCurrentPanel = dummy;

    connect(&myFiltersMapper, SIGNAL(mapped(QObject*)), this, SLOT(onFilterChange(QObject*)));
    updateFilters();
    connect(&myReportsMapper, SIGNAL(mapped(QObject*)), this, SLOT(onReportChange(QObject*)));
    updateReports();

    addPanel("Archetype", new ArchetypePanel(myStore, myResources, this));
    addPanel("Face", new CREFacePanel(this, model, myResources, myStore));
    addPanel("Animation", new AnimationPanel(this, model));
    addPanel("Artifact", new CREArtifactPanel(this));
    addPanel("ArtifactList", new ArtifactListPanel(this));
    addPanel("Recipe", new RecipePanel(this));
    addPanel("Treasure", new TreasurePanel(this));
    addPanel("TreasureList", new CRETreasurePanel(this));
    addPanel("Faceset", new FacesetsPanel(this));
    addPanel("Quest", new CREQuestPanel(myStore, myMessages, myResources, model, this));
    addPanel("GeneralMessage", new CREGeneralMessagePanel(this));
    addPanel("Region", new RegionPanel(this));
    addPanel("Map", new CREMapPanel(myScripts, this));
    addPanel("Script", new CREScriptPanel(this));
    addPanel("Message", new CREMessagePanel(myMessages, this));
    addPanel("RandomMap", new CRERandomMapPanel(this));

    splitter->setSizes({5000, 5000});

    QApplication::restoreOverrideCursor();
}

CREResourcesWindow::~CREResourcesWindow()
{
    qDeleteAll(myPanels);
}

void CREResourcesWindow::commitData()
{
    if (myCurrentPanel != NULL)
        myCurrentPanel->commitData();
}

void CREResourcesWindow::currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (previous.isValid()) {
        commitData();
    }

    if (!current.isValid()) {
        myCurrentPanel = nullptr;
        return;
    }

    auto rc = myModel->mapToSource(current);
    AssetWrapper *item = reinterpret_cast<AssetWrapper *>(rc.internalPointer());
    if (!item) {
        return;
    }

    CREPanel* newPanel = myPanels[item->displayPanelName()];
    if (!newPanel) {
//        printf("no panel for %s\n", qPrintable(item->getPanelName()));
        return;
    }

    item->displayFillPanel(newPanel);

    if (myCurrentPanel != newPanel) {
        myStackedPanels->setCurrentWidget(newPanel);
        myCurrentPanel = newPanel;
    }
}

void CREResourcesWindow::addPanel(QString name, CREPanel* panel)
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

    if (myFilters.filters().size() > 0)
    {
        QAction* clear = new QAction(tr("(none)"), this);
        connect(clear, SIGNAL(triggered()), this, SLOT(clearFilter()));
        myFiltersMenu->addAction(clear);

        foreach(CREFilterDefinition* filter, myFilters.filters())
        {
            QAction* a = new QAction(filter->name(), this);
            myFiltersMenu->addAction(a);
            myFiltersMapper.setMapping(a, filter);
            connect(a, SIGNAL(triggered()), &myFiltersMapper, SLOT(map()));
        }

        myFiltersMenu->addSeparator();
    }

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
    bool ok;
    QString filter = QInputDialog::getText(this, tr("Quick filter"), tr("Filter:"), QLineEdit::Normal, myModel->filter(), &ok);
    if (!ok)
        return;
    setFilter(filter, filter);
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
