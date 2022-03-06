#include "ObjectWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"

ObjectWrapper::ObjectWrapper(AssetWrapper *parent, object *ob, ResourcesManager *resourcesManager) : AssetTWrapper(parent, "empty", ob) {
    myResourcesManager = resourcesManager;
}

ObjectWrapper::~ObjectWrapper() {
}

AssetWrapper* ObjectWrapper::arch()
{
    return myResourcesManager->wrap(myWrappedItem->arch, this);
}

QString ObjectWrapper::name() const
{
    return myWrappedItem->name;
}

QString ObjectWrapper::race() const
{
    return myWrappedItem->race;
}

int ObjectWrapper::type() const
{
    return myWrappedItem->type;
}

int ObjectWrapper::level() const
{
    return myWrappedItem->level;
}

double scale_level(double l) {
    if (l < 1) {
        l = 1;
    }
    return 10*log(l)+1;
}

int ObjectWrapper::suggested_level() const {
    return scale_level(hp()*10/100 + regen()*20/100 + dps()*20/100);
}

bool ObjectWrapper::isMonster() const
{
    return QUERY_FLAG(myWrappedItem, FLAG_MONSTER);
}

bool ObjectWrapper::isAlive() const
{
    return QUERY_FLAG(myWrappedItem, FLAG_ALIVE);
}

quint32 ObjectWrapper::attacktype() const
{
    return myWrappedItem->attacktype;
}

QString ObjectWrapper::materialName() const
{
  return myWrappedItem->materialname;
}

float ObjectWrapper::dps() const {
    return damage() * FABS(speed()) / MOVE_PER_SECOND;
}

float ObjectWrapper::regen() const {
    return con() / 4.0 / MOVE_PER_SECOND;
}

AssetWrapper *ObjectWrapper::randomItems() {
    if (!myWrappedItem->randomitems) {
        return nullptr;
    }
    return myResourcesManager->wrap(myWrappedItem->randomitems, nullptr);
}
