#include "ObjectWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"

ObjectWrapper::ObjectWrapper(AssetWrapper *parent, object *ob, ResourcesManager *resourcesManager) : AssetTWrapper(parent, "empty", ob) {
    myResourcesManager = resourcesManager;
}

ObjectWrapper::~ObjectWrapper() {
}

/*
void CREWrapperObject::setObject(object* obj)
{
    myItem = obj;
    if (myArchetype == NULL)
        myArchetype = new ArchetypeWrapper(this, obj->arch);
    else
        myArchetype->setArchetype(obj->arch);
    delete myTreasure;
    myTreasure = nullptr;
}
 */

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

AssetWrapper *ObjectWrapper::randomItems() {
    if (!myItem->randomitems) {
        return nullptr;
    }
    return myResourcesManager->wrap(myItem->randomitems, nullptr);
}
