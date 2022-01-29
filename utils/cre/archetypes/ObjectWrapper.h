#ifndef CRE_WRAPPER_OBJECT_h
#define CRE_WRAPPER_OBJECT_h

extern "C" {
#include "global.h"
}

#include <QObject>

#include "assets/AssetWrapper.h"

class ResourcesManager;
#include "treasures/TreasureListWrapper.h"

class ObjectWrapper : public AssetTWrapper<object> {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString race READ race)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString slaying READ slaying)
    Q_PROPERTY(QString skill READ skill)
    Q_PROPERTY(int type READ type)
    Q_PROPERTY(int subType READ subType)
    Q_PROPERTY(int level READ level WRITE setLevel)
    Q_PROPERTY(bool isMonster READ isMonster)
    Q_PROPERTY(bool isAlive READ isAlive)
    Q_PROPERTY(bool isGenerator READ isGenerator)
    Q_PROPERTY(qint64 experience READ experience WRITE setExperience)
    Q_PROPERTY(quint32 attacktype READ attacktype)
    Q_PROPERTY(int ac READ ac WRITE setAc)
    Q_PROPERTY(int con READ con WRITE setCon)
    Q_PROPERTY(int wc READ wc WRITE setWc)
    Q_PROPERTY(QObject* arch READ arch)
    Q_PROPERTY(int damage READ damage WRITE setDamage)
    Q_PROPERTY(int hp READ hp WRITE setHp)
    Q_PROPERTY(int weight READ weight WRITE setWeight)
    Q_PROPERTY(QString materialName READ materialName)
    Q_PROPERTY(QObject* randomItems READ randomItems)
    Q_PROPERTY(float speed READ speed WRITE setSpeed);
    Q_PROPERTY(float dps READ dps);
    Q_PROPERTY(float regen READ regen);

    public:
        ObjectWrapper(AssetWrapper *parent, object *ob, ResourcesManager *resourcesManager);
        virtual ~ObjectWrapper();

        virtual QString displayName() const override { return name(); }

        object *getObject() { return myWrappedItem; }

        AssetWrapper* arch();
        QString name() const;
        QString race() const;
        QString title() const { return myWrappedItem->title; }
        QString slaying() const { return myWrappedItem->slaying; }
        QString skill() const { return myWrappedItem->skill; }
        int type() const;
        int subType() const { return myWrappedItem->subtype; }
        int level() const;
        void setLevel(int level) { myWrappedItem->level = level; }
        bool isMonster() const;
        bool isAlive() const;
        bool isGenerator() const { return QUERY_FLAG(myWrappedItem, FLAG_GENERATOR); }
        int64_t experience() const { return myWrappedItem->stats.exp; }
        void setExperience(int64_t experience) { myWrappedItem->stats.exp = experience; }
        quint32 attacktype() const;
        int8_t ac() const { return myWrappedItem->stats.ac; }
        void setAc(int8_t ac) { myWrappedItem->stats.ac = ac; }
        int8_t wc() const { return myWrappedItem->stats.wc; }
        void setWc(int8_t wc) { myWrappedItem->stats.wc = wc; }
        int16_t damage() const { return myWrappedItem->stats.dam; }
        int8_t con() const { return myWrappedItem->stats.Con; }
        void setCon(int8_t con) { myWrappedItem->stats.Con = con; }
        void setDamage(int16_t damage) { myWrappedItem->stats.dam = damage; }
        int16_t hp() const { return myWrappedItem->stats.hp; }
        void setHp(int16_t hp) { myWrappedItem->stats.hp = hp; }
        int32_t weight() const { return myWrappedItem->weight; }
        void setWeight(int32_t weight) { myWrappedItem->weight = weight; }
        QString materialName() const;
        AssetWrapper *randomItems();
        float speed() const { return myWrappedItem->speed; }
        void setSpeed(float speed) { myWrappedItem->speed = speed; }
        const Face *face() const { return myWrappedItem->face; }
        float dps() const;
        float regen() const;

    protected:
        ResourcesManager *myResourcesManager;
};

#endif // CRE_WRAPPER_OBJECT_h
