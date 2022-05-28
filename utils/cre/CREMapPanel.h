#ifndef _CREMAPPANEL_H
#define _CREMAPPANEL_H

#include <QObject>
#include <QtWidgets>
#include "assets/AssetWrapperPanel.h"

class CREMapInformation;
class ScriptFileManager;

class CREMapPanel : public AssetSWrapperPanel<CREMapInformation>
{
    public:
        CREMapPanel(ScriptFileManager* manager, QWidget* parent);
        virtual ~CREMapPanel();

        virtual void updateItem() override;

    protected:
        ScriptFileManager* myManager;
        QLabel* myName;
        QTreeWidget* myExitsTo;
        QTreeWidget* myExitsFrom;
        QTreeWidget* myScripts;
        QLabel* myBackgroundMusic;
};

#endif /* _CREMAPPANEL_H */
