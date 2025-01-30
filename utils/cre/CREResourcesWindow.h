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

#ifndef CRERESOURCESWINDOW_H
#define CRERESOURCESWINDOW_H

#include <QObject>
#include <QtWidgets>

#include "CREFilterDefinitionManager.h"
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

    signals:
        void filtersModified();

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
        CREScriptEngine myEngine;

        void addPanel(QString name, AssetWrapperPanel* panel);
        void setFilter(const QString &filter, const QString &name);

    protected slots:
        void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
        void onFilter();
        void onFilterChange(QObject* object);
        void onQuickFilter();
        void clearFilter();
        void treeCustomMenu(const QPoint & pos);
        void addQuest(bool);
        void addMessage(bool);
        void onExportAsCsv();
};

#endif // CRERESOURCESWINDOW_H
