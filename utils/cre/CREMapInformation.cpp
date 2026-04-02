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

#include "CREMapInformation.h"
#include "random_maps/RandomMap.h"
#include "quests/QuestWrapper.h"
#include "faces/FaceWrapper.h"
#include "animations/AnimationWrapper.h"
#include "scripts/ScriptFile.h"
#include "archetypes/ArchetypeWrapper.h"

CREMapInformation::CREMapInformation() : AssetWrapper(nullptr, "Map")
{
    myDifficulty = 0;
    myComputedDifficulty = 0;
    myExperience = 0;
    myShopGreed = 0;
    myShopMin = 0;
    myShopMax = 0;
}

CREMapInformation::CREMapInformation(const QString& path) : AssetWrapper(nullptr, "Map")
{
    myPath = path;
    myDifficulty = 0;
    myComputedDifficulty = 0;
    myExperience = 0;
    myShopGreed = 0;
    myShopMin = 0;
    myShopMax = 0;
}

CREMapInformation::~CREMapInformation()
{
    qDeleteAll(myRandomMaps);
}

QString CREMapInformation::displayName() const
{
    if (myName.isEmpty())
        return myPath;
    return myName;
}

const QString& CREMapInformation::path() const
{
    return myPath;
}

void CREMapInformation::setPath(const QString& path)
{
    myPath = path;
}

const QString& CREMapInformation::name() const
{
    return myName;
}

void CREMapInformation::setName(const QString& name)
{
    myName = name;
}

const QString& CREMapInformation::backgroundMusic()
{
    return myBackgroundMusic;
}

void CREMapInformation::setBackgroundMusic(const QString& music)
{
    myBackgroundMusic = music;
}

const QString& CREMapInformation::resetGroup() const
{
    return myResetGroup;
}

void CREMapInformation::setResetGroup(const QString& resetGroup)
{
    myResetGroup = resetGroup;
}

QStringList CREMapInformation::archetypes() const
{
    return myArchetypes;
}

void CREMapInformation::addArchetype(const QString& archetype)
{
    if (!myArchetypes.contains(archetype))
        myArchetypes.append(archetype);
}

QStringList CREMapInformation::faces() const
{
    return myFaces.values();
}

void CREMapInformation::addFace(const QString& face)
{
    myFaces.insert(face);
}

QStringList CREMapInformation::animations() const
{
    return myAnimations.values();
}

void CREMapInformation::addAnimation(const QString& anim)
{
    myAnimations.insert(anim);
}

const QDateTime& CREMapInformation::mapTime() const
{
    return myMapTime;
}

void CREMapInformation::setMapTime(const QDateTime& date)
{
    myMapTime = date;
}

QStringList CREMapInformation::exitsTo() const
{
    return myExitsTo;
}

void CREMapInformation::addExitTo(const QString& path)
{
    if (!myExitsTo.contains(path))
        myExitsTo.append(path);
}

QStringList CREMapInformation::accessedFrom() const
{
    return myAccessedFrom;
}

void CREMapInformation::addAccessedFrom(const QString& path)
{
    if (!myAccessedFrom.contains(path))
        myAccessedFrom.append(path);
}

int CREMapInformation::difficulty() const
{
    return myDifficulty;
}

void CREMapInformation::setDifficulty(int difficulty)
{
    myDifficulty = difficulty;
}

int CREMapInformation::computedDifficulty() const
{
    return myComputedDifficulty;
}

void CREMapInformation::setComputedDifficulty(int computedDifficulty)
{
    myComputedDifficulty = computedDifficulty;
}

qint64 CREMapInformation::experience() const
{
    return myExperience;
}

void CREMapInformation::setExperience(qint64 experience)
{
    myExperience = experience;
}

const QString& CREMapInformation::region() const
{
    return myRegion;
}
void CREMapInformation::setRegion(const QString& region)
{
    myRegion = region;
}

QStringList CREMapInformation::messages() const
{
    return myMessages;
}

void CREMapInformation::addMessage(const QString& message)
{
    if (!myMessages.contains(message))
        myMessages.append(message);
}

QStringList CREMapInformation::quests() const
{
    return myQuests;
}

void CREMapInformation::addQuest(const QString& quest)
{
    if (!myQuests.contains(quest))
        myQuests.append(quest);
}

QHash<QString, int>& CREMapInformation::shopItems()
{
    return myShopItems;
}

const QHash<QString, int>& CREMapInformation::shopItems() const
{
    return myShopItems;
}

double CREMapInformation::shopGreed() const
{
    return myShopGreed;
}

void CREMapInformation::setShopGreed(double greed)
{
    myShopGreed = greed;
}

const QString& CREMapInformation::shopRace() const
{
  return myShopRace;
}

void CREMapInformation::setShopRace(const QString& race)
{
  myShopRace = race;
}

quint64 CREMapInformation::shopMin() const
{
  return myShopMin;
}

void CREMapInformation::setShopMin(quint64 min)
{
  myShopMin = min;
}

quint64 CREMapInformation::shopMax() const
{
  return myShopMax;
}

void CREMapInformation::setShopMax(quint64 max)
{
  myShopMax = max;
}

QList<RandomMap*> CREMapInformation::randomMaps() const
{
  return myRandomMaps;
}

void CREMapInformation::addRandomMap(RandomMap* map)
{
  myRandomMaps.append(map);
}

AssetWrapper::PossibleUse CREMapInformation::uses(const AssetWrapper *asset, std::string &hint) const {
    auto quest = dynamic_cast<const QuestWrapper *>(asset);
    if (quest) {
        return myQuests.indexOf(quest->wrappedItem()->quest_code) == -1 ? DoesntUse : Uses;
    }
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        return myFaces.find(face->wrappedItem()->name) == myFaces.end() ? DoesntUse : Uses;
    }
    auto anim = dynamic_cast<const AnimationWrapper *>(asset);
    if (anim) {
        return myAnimations.find(anim->wrappedItem()->name) == myAnimations.end() ? DoesntUse : Uses;
    }
    auto script = dynamic_cast<const ScriptFile *>(asset);
    if (script) {
        return script->forMap(this) ? Uses : DoesntUse;
    }
    auto arch = dynamic_cast<const ArchetypeWrapper *>(asset);
    if (arch) {
        if (myArchetypes.indexOf(arch->wrappedItem()->name) != -1) {
            return Uses;
        };
        for (auto map : myRandomMaps) {
            if (strcmp(arch->wrappedItem()->name, map->parameters()->final_exit_archetype) == 0) {
                hint = "random map final exit";
                return Uses;
            }
        }
        return DoesntUse;
    }
    return DoesntUse;
}
