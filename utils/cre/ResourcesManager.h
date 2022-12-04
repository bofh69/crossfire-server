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

#ifndef RESOURCESMANAGER_H
#define RESOURCESMANAGER_H

#include "global.h"
#include "object.h"
#include "quest.h"
#include "AssetsTracker.h"
#include <set>
#include <QObject>
#include "ModifiedAssetsManager.h"
#include "LicenseManager.h"

#include "animations/AnimationWrapper.h"
#include "archetypes/ArchetypeWrapper.h"
#include "archetypes/ObjectWrapper.h"
#include "faces/FaceWrapper.h"
#include "faces/FacesetsWrapper.h"
#include "general_messages/GeneralMessageWrapper.h"
#include "quests/QuestWrapper.h"
#include "regions/RegionWrapper.h"
#include "treasures/TreasureListWrapper.h"
#include "treasures/TreasureWrapper.h"
#include "artifacts/ArtifactWrapper.h"
#include "artifacts/ArtifactListWrapper.h"
#include "recipes/RecipeListWrapper.h"
#include "recipes/RecipeWrapper.h"

class CREMapInformation;
class CREMapInformationManager;
class AssetWrapper;

class ResourcesManager;

template<class A, class W>
class AssetWrapperManager {
public:
    W *wrap(A *asset, AssetWrapper *parent, ResourcesManager *resources) {
        if (!myWrappedAssets.contains(asset)) {
            myWrappedAssets[asset] = new W(parent, asset, resources);
        }
        return myWrappedAssets[asset];
    }
    void remove(A *asset) {
        auto found = myWrappedAssets.find(asset);
        if (found != myWrappedAssets.end()) {
            delete found.value();
            myWrappedAssets.erase(found);
        }
    }
private:
    QHash<A *, W *> myWrappedAssets;
};

enum ArchetypeUse {
  OTHER_ARCH,
  DEATH_ANIM,
  TREASURE_USE,
  MAP_USE,
  RANDOM_MAP_FINAL_EXIT,
  ALCHEMY_PRODUCT,
};

typedef std::function<bool(ArchetypeUse use, const archetype*, const treasurelist*, const CREMapInformation*, recipe*)> AssetUseCallback;

/**
 * Class managing all assets, tracking in which file they are, which are modified, and such.
 */
class ResourcesManager : public QObject, AssetsTracker
{
    Q_OBJECT

    public:
        ResourcesManager();
        virtual ~ResourcesManager();

        void setMapInformationManager(CREMapInformationManager *mapInformationManager) {
            myMapInformationManager = mapInformationManager;
        }
        CREMapInformationManager *getMapInformationManager() {
            return myMapInformationManager;
        }

        void load();

        int recipeMaxIngredients() const;
        QStringList recipes(int count) const;
        const recipe* getRecipe(int ingredients, const QString& name) const;

        virtual void assetDefined(const archetype *arch, const std::string &filename) override { myArchetypes.assetDefined(arch, filename); }
        virtual void assetDefined(const quest_definition *asset, const std::string &filename) override { myQuests.assetDefined(asset, filename); }
        virtual void assetDefined(const treasurelist *asset, const std::string &filename) override { myTreasures.assetDefined(asset, filename); }
        virtual void assetDefined(const GeneralMessage *asset, const std::string &filename) override { myGeneralMessages.assetDefined(asset, filename); }
        virtual void assetDefined(const artifact *asset, const std::string &filename) override { myArtifacts.assetDefined(asset, filename); }
        virtual void assetDefined(const Face *asset, const std::string &filename) override { myFaces.assetDefined(asset, filename); }
        virtual void assetDefined(const Animations *asset, const std::string &filename) override { myAnimations.assetDefined(asset, filename); }

        const std::map<std::string, std::set<const archetype*> >& origins() const { return myArchetypes.origins(); }
        std::string originOf(const archetype *arch) const { return myArchetypes.originOf(arch); }

        const std::map<std::string, std::set<const quest_definition*> >& questOrigins() const { return myQuests.origins(); }
        std::string originOfQuest(const quest_definition *quest) const { return myQuests.originOf(quest); }
        std::vector<std::string> questFiles() const { return myQuests.files(); }

        static void archetypeUse(const archetype* item, CREMapInformationManager* store, AssetUseCallback callback);

        std::string originOf(const Face *face) const { return myFaces.originOf(face); }
        std::string originOf(const artifact *art) const { return myArtifacts.originOf(art); }

        bool hasPendingChanges() const {
            return
                    myArchetypes.hasPendingChanges()
                    || myQuests.hasPendingChanges()
                    || myTreasures.hasPendingChanges()
                    || myGeneralMessages.hasPendingChanges()
                    || myArtifacts.hasPendingChanges()
                    ;
        }

        ArchetypeWrapper *wrap(archetype *arch, AssetWrapper *parent) { return myWrappedArchetypes.wrap(arch, parent, this); }
        ObjectWrapper *wrap(object *ob, AssetWrapper *parent) { return myWrappedObjects.wrap(ob, parent, this); }
        AssetWrapper *wrap(Face *face, AssetWrapper *parent) { return myWrappedFaces.wrap(face, parent, this); }
        AssetWrapper *wrap(Animations *anim, AssetWrapper *parent) { return myWrappedAnimations.wrap(anim, parent, this); }
        AssetWrapper *wrap(treasurelist *tr, AssetWrapper *parent) { return myWrappedTreasureLists.wrap(tr, parent, this); }
        AssetWrapper *wrap(treasure *tr, AssetWrapper *parent) { return myWrappedTreasures.wrap(tr, parent, this); }
        AssetWrapper *wrap(region *reg, AssetWrapper *parent) { return myWrappedRegions.wrap(reg, parent, this); }
        AssetWrapper *wrap(face_sets *fs, AssetWrapper *parent) { return myWrappedFacesets.wrap(fs, parent, this); }
        AssetWrapper *wrap(GeneralMessage *message, AssetWrapper *parent) { return myWrappedMessages.wrap(message, parent, this); }
        AssetWrapper *wrap(quest_definition *quest, AssetWrapper *parent) { return myWrappedQuests.wrap(quest, parent, this); }
        AssetWrapper *wrap(quest_step_definition *step, AssetWrapper *parent) { return myWrappedQuestSteps.wrap(step, parent, this); }
        AssetWrapper *wrap(artifactlist *al, AssetWrapper *parent) { return myWrappedArtifactLists.wrap(al, parent, this); }
        AssetWrapper *wrap(artifact *art, AssetWrapper *parent) {
            auto wrapper = myWrappedArtifacts.wrap(art, parent, this);
            connect(wrapper, &AssetWrapper::modified, [wrapper, this] { myArtifacts.assetModified(wrapper->wrappedItem()); });
            return wrapper;
        }
        AssetWrapper *wrap(recipelist *list, AssetWrapper *parent) { return myWrappedRecipeLists.wrap(list, parent, this); }
        AssetWrapper *wrap(recipe *rc, AssetWrapper *parent) { return myWrappedRecipes.wrap(rc, parent, this); }
        void remove(treasure *tr) { myWrappedTreasures.remove(tr); }

        LicenseManager *licenseManager() { return &myLicenseManager; }

    public slots:
      void archetypeModified(archetype *arch);
      void saveArchetypes();
      void questModified(quest_definition *quest);
      void saveQuests();
      void treasureModified(treasurelist *treasure);
      void saveTreasures();
      void generalMessageModified(GeneralMessage *message);
      void saveGeneralMessages();
      void saveArtifacts();

    protected:
        CREMapInformationManager *myMapInformationManager;
        QList<QHash<QString, recipe*> > myRecipes;
        ModifiedAssetsManager<archetype> myArchetypes;
        ModifiedAssetsManager<quest_definition> myQuests;
        ModifiedAssetsManager<treasurelist> myTreasures;
        ModifiedAssetsManager<GeneralMessage> myGeneralMessages;
        ModifiedAssetsManager<artifact> myArtifacts;
        ModifiedAssetsManager<Face> myFaces;
        ModifiedAssetsManager<Animations> myAnimations;
        AssetWrapperManager<archetype, ArchetypeWrapper> myWrappedArchetypes;
        AssetWrapperManager<object, ObjectWrapper> myWrappedObjects;
        AssetWrapperManager<Face, FaceWrapper> myWrappedFaces;
        AssetWrapperManager<Animations, AnimationWrapper> myWrappedAnimations;
        AssetWrapperManager<treasurelist, TreasureListWrapper> myWrappedTreasureLists;
        AssetWrapperManager<treasure, TreasureWrapper> myWrappedTreasures;
        AssetWrapperManager<region, RegionWrapper> myWrappedRegions;
        AssetWrapperManager<face_sets, FacesetsWrapper> myWrappedFacesets;
        AssetWrapperManager<GeneralMessage, GeneralMessageWrapper> myWrappedMessages;
        AssetWrapperManager<quest_definition, QuestWrapper> myWrappedQuests;
        AssetWrapperManager<quest_step_definition, QuestStepWrapper> myWrappedQuestSteps;
        AssetWrapperManager<artifactlist, ArtifactListWrapper> myWrappedArtifactLists;
        AssetWrapperManager<artifact, ArtifactWrapper> myWrappedArtifacts;
        AssetWrapperManager<recipelist, RecipeListWrapper> myWrappedRecipeLists;
        AssetWrapperManager<recipe, RecipeWrapper> myWrappedRecipes;
        LicenseManager myLicenseManager;
};

#endif /* RESOURCESMANAGER_H */
