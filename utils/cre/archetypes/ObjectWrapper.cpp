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
    return myResourcesManager->wrap(myItem->arch, this);
}

QString ObjectWrapper::name() const
{
    return myItem->name;
}

QString ObjectWrapper::race() const
{
    return myItem->race;
}

int ObjectWrapper::type() const
{
    return myItem->type;
}

int ObjectWrapper::level() const
{
    return myItem->level;
}

bool ObjectWrapper::isMonster() const
{
    return QUERY_FLAG(myItem, FLAG_MONSTER);
}

bool ObjectWrapper::isAlive() const
{
    return QUERY_FLAG(myItem, FLAG_ALIVE);
}

quint32 ObjectWrapper::attacktype() const
{
    return myItem->attacktype;
}

QString ObjectWrapper::materialName() const
{
  return myItem->materialname;
}

float ObjectWrapper::dps() const {
    return damage() * speed() / MOVE_PER_SECOND;
}

float ObjectWrapper::regen() const {
    return con() / 4.0 / MOVE_PER_SECOND;
}

AssetWrapper *ObjectWrapper::randomItems() {
    if (!myItem->randomitems) {
        return nullptr;
    }
    return myResourcesManager->wrap(myItem->randomitems, nullptr);
}
