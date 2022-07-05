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

#ifndef CLASS_CRE_MAP_INFORMATION_H
#define CLASS_CRE_MAP_INFORMATION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QSet>

#include "assets/AssetWrapper.h"

class RandomMap;

class CREMapInformation : public AssetWrapper
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int difficulty READ difficulty)
    Q_PROPERTY(int computedDifficulty READ computedDifficulty)
    Q_PROPERTY(qint64 experience READ experience)
    Q_PROPERTY(QString resetGroup READ resetGroup)

    public:
        CREMapInformation();
        CREMapInformation(const QString& path);
        virtual ~CREMapInformation();

        virtual QString displayName() const override;

        virtual PossibleUse uses(const AssetWrapper *asset, std::string &) const override;

        const QString& path() const;
        void setPath(const QString& path);

        const QString& name() const;
        void setName(const QString& name);

        const QString& backgroundMusic();
        void setBackgroundMusic(const QString& music);

        const QString& resetGroup() const;
        void setResetGroup(const QString& resetGroup);

        QStringList archetypes() const;
        void addArchetype(const QString& archetype);

        QStringList faces() const;
        void addFace(const QString& face);

        QStringList animations() const;
        void addAnimation(const QString& anim);

        const QDateTime& mapTime() const;
        void setMapTime(const QDateTime& date);

        QStringList exitsTo() const;
        void addExitTo(const QString& path);

        QStringList accessedFrom() const;
        void addAccessedFrom(const QString& path);

        int difficulty() const;
        void setDifficulty(int difficulty);

        int computedDifficulty() const;
        void setComputedDifficulty(int computed);

        qint64 experience() const;
        void setExperience(qint64 experience);

        const QString& region() const;
        void setRegion(const QString& region);

        QStringList messages() const;
        void addMessage(const QString& message);

        QStringList quests() const;
        void addQuest(const QString& quest);

        QHash<QString, int>& shopItems();
        const QHash<QString, int>& shopItems() const;

        double shopGreed() const;
        void setShopGreed(double greed);
        const QString& shopRace() const;
        void setShopRace(const QString& race);
        quint64 shopMin() const;
        void setShopMin(quint64 min);
        quint64 shopMax() const;
        void setShopMax(quint64 max);

        QList<RandomMap*> randomMaps() const;
        void addRandomMap(RandomMap* map);

    protected:
        QString myPath;
        QString myName;
        QStringList myArchetypes;
        QSet<QString> myFaces;
        QSet<QString> myAnimations;
        QDateTime myMapTime;
        QStringList myExitsTo;
        QStringList myAccessedFrom;
        int myDifficulty;
        int myComputedDifficulty;
        qint64 myExperience;
        QString myRegion;
        QStringList myMessages;
        QStringList myQuests;
        QHash<QString, int> myShopItems;
        double myShopGreed;
        QString myShopRace;
        quint64 myShopMin, myShopMax;
        QList<RandomMap*> myRandomMaps;
        QString myBackgroundMusic;
        QString myResetGroup;
};

#endif // CLASS_CRE_MAP_INFORMATION_H
