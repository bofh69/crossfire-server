#ifndef SCRIPT_FILE_H
#define SCRIPT_FILE_H

#include <QList>
#include "assets/AssetWrapper.h"
#include "assets/AssetWrapperPanel.h"

class CREMapInformation;

/** Information about an event hook, linked to a script file. */
class HookInformation {
public:
    HookInformation(CREMapInformation* map, int x, int y, const QString& itemName, const QString& pluginName, const QString& eventName);

    const CREMapInformation* map() const;
    int x() const;
    int y() const;
    QString itemName() const;
    QString pluginName() const;
    QString eventName() const;

private:
    CREMapInformation* myMap;
    int myX;
    int myY;
    QString myItemName;
    QString myPluginName;
    QString myEventName;
};

/** Information about a script file. */
class ScriptFile : public AssetWrapper {
public:
    ScriptFile(AssetWrapper *parent, const QString& path);
    virtual ~ScriptFile();

    virtual QString displayName() const override { return myPath; }
    virtual void displayFillPanel(QWidget *panel) override {
        AssetWrapperPanel *p = static_cast<AssetWrapperPanel *>(panel);
        p->setItem(this);
    }

    void addHook(HookInformation* hook);
    bool forMap(const CREMapInformation* map) const;
    /**
     * Remove hooks linked to a map.
     * @param map map to remove scripts of.
     * @return true if this script has no more hooks, false else.
     */
    bool removeMap(CREMapInformation* map);
    QList<HookInformation*> hooks() const;
    const QString& path() const;

private:
    QString myPath;
    QList<HookInformation*> myHooks;
};

#endif /* SCRIPT_FILE_H */
