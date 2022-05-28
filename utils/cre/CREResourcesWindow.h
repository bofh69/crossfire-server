#ifndef CRERESOURCESWINDOW_H
#define CRERESOURCESWINDOW_H

#include <QObject>
#include <QtWidgets>

#include "CREFilterDefinitionManager.h"
#include "CREReportDefinitionManager.h"
#include "CREScriptEngine.h"

class CREMapInformationManager;
class MessageManager;
class ResourcesManager;
class ScriptFileManager;
class AssetModel;
class ScriptFilterAssetModel;
class AssetWrapperPanel;

class CREResourcesWindow : public QWidget
{
    Q_OBJECT

    public:
        CREResourcesWindow(CREMapInformationManager* store, MessageManager* messages, ResourcesManager* resources, ScriptFileManager* scripts, AssetModel *model, const QModelIndex &root, QWidget* parent);
        virtual ~CREResourcesWindow();

        int rootIndex() const { return myTreeRoot.isValid() ? myTreeRoot.row() : -1; }

    public slots:
        void updateFilters();
        void updateReports();
        void commitData();

    signals:
        void filtersModified();
        void reportsModified();

    protected:
        ScriptFilterAssetModel *myModel;
        QTreeView* myTree;
        QModelIndex myTreeRoot;
        AssetWrapperPanel* myCurrentPanel;
        QHash<QString, QPointer<AssetWrapperPanel> > myPanels;
        QStackedLayout* myStackedPanels;
        CREMapInformationManager* myStore;
        MessageManager* myMessages;
        ResourcesManager* myResources;
        ScriptFileManager* myScripts;
        QPushButton* myFilterButton;
        QMenu* myFiltersMenu;
        QSignalMapper myFiltersMapper;
        CREFilterDefinitionManager myFilters;
        QMenu* myReportsMenu;
        QSignalMapper myReportsMapper;
        CREReportDefinitionManager myReports;
        CREScriptEngine myEngine;

        void addPanel(QString name, AssetWrapperPanel* panel);
        void setFilter(const QString &filter, const QString &name);

    protected slots:
        void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
        void onFilter();
        void onFilterChange(QObject* object);
        void onQuickFilter();
        void clearFilter();
        void onReport();
        void onReportChange(QObject* object);
        void treeCustomMenu(const QPoint & pos);
        void addQuest(bool);
        void addMessage(bool);
};

#endif // CRERESOURCESWINDOW_H
