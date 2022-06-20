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

#include <QMainWindow>

#include "CREResourcesWindow.h"

class QMdiArea;
class QAction;
class QMenu;
class QLabel;
class CREArtifactWindow;
class CREArchetypeWindow;
class CRETreasureWindow;
class CREAnimationWindow;
class CREFormulaeWindow;
class CREMapInformation;
class CREMapInformationManager;
class QuestManager;
class MessageManager;
class ResourcesManager;
class ScriptFileManager;
class AssetWrapper;
class AllAssets;
class AssetModel;
class ChangesDock;
class HelpManager;

class CREMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        CREMainWindow(const QString &helpRoot);

    signals:
        void updateFilters();
        void updateReports();

    private:
        QMdiArea* myArea;
        AssetModel *myModel;
        AllAssets *myAssets;

        void createActions();
        void createMenus();

        QMenu* myOpenMenu;
        QMenu* mySaveMenu;

        QAction* mySaveFormulae;
        QAction* myReportPlayer;
        QAction* myReportShops;
        QAction *myReportQuests;
        QAction* myReportArchetypes;
        QMenu* myToolsMenu;
        QActionGroup *myFacesetsGroup;
        QAction* myToolFacesetUseFallback;
        QAction* myClearMapCache;
        QMenu *myWindows;
        QLabel* myMapBrowseStatus;
        CREMapInformationManager* myMapManager;
        MessageManager* myMessageManager;
        ResourcesManager* myResourcesManager;
        ScriptFileManager* myScriptManager;
        ChangesDock* myChanges;
        HelpManager* myHelpManager;

    protected:
        void closeEvent(QCloseEvent* event);
        void doResourceWindow(int assets, const QByteArray& position = QByteArray());
        void fillFacesets();
        QAction *createAction(const QString &title, const QString &statusTip);
        QAction *createAction(const QString &title, const QString &statusTip, QObject *target, const char *slot);

    private slots:
        void onOpenExperience(const QByteArray& position = QByteArray());
        void onSaveFormulae();
        void onSaveQuests();
        void onSaveMessages();
        void onReportDuplicate();
        void onReportSpellDamage();
        void onReportAlchemy();
        void onReportAlchemyGraph();
        void onReportSpells();
        void onReportPlayer();
        void onReportSummon();
        void onReportShops();
        void onReportQuests();
        void onReportMaterials();
        void onReportArchetypes();
        void onReportLicenses();
        void onToolEditMonsters();
        void onToolSmooth();
        void onToolCombatSimulator();
        void onToolBarMaker();
        void onToolFaceMaker();
        void onToolFaceset(QAction* action);
        void onToolFacesetUseFallback();
        void onToolReloadAssets();
        void onWindowsShowing();
        void browsingMap(const QString& path);
        void browsingFinished();
        void onFiltersModified();
        void onReportsModified();
        void onClearCache();

        void mapAdded(CREMapInformation *map);
};
