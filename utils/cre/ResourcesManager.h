#ifndef RESOURCESMANAGER_H
#define RESOURCESMANAGER_H

extern "C" {
#include "global.h"
#include "object.h"
#include "quest.h"
}
#include "AssetsTracker.h"
#include <set>
#include <QObject>
#include "ModifiedAssetsManager.h"

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

typedef std::function<bool(ArchetypeUse use, const archt*, const treasurelist*, const CREMapInformation*, recipe*)> AssetUseCallback;

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
        const recipestruct* getRecipe(int ingredients, const QString& name) const;

        virtual void assetDefined(const archt *arch, const std::string &filename) override { myArchetypes.assetDefined(arch, filename); }
        virtual void assetDefined(const quest_definition *asset, const std::string &filename) override { myQuests.assetDefined(asset, filename); }
        virtual void assetDefined(const treasurelist *asset, const std::string &filename) override { myTreasures.assetDefined(asset, filename); }
        virtual void assetDefined(const GeneralMessage *asset, const std::string &filename) override { myGeneralMessages.assetDefined(asset, filename); }

        const std::map<std::string, std::set<const archt*> >& origins() const { return myArchetypes.origins(); }
        std::string originOf(const archt *arch) const { return myArchetypes.originOf(arch); }

        const std::map<std::string, std::set<const quest_definition*> >& questOrigins() const { return myQuests.origins(); }
        std::string originOfQuest(const quest_definition *quest) const { return myQuests.originOf(quest); }

        static void archetypeUse(const archt* item, CREMapInformationManager* store, AssetUseCallback callback);

        bool hasPendingChanges() const {
            return
                    myArchetypes.hasPendingChanges()
                    || myQuests.hasPendingChanges()
                    || myTreasures.hasPendingChanges()
                    || myGeneralMessages.hasPendingChanges()
                    ;
        }

        AssetWrapper *wrap(archt *arch, AssetWrapper *parent) { return myWrappedArchetypes.wrap(arch, parent, this); }
        AssetWrapper *wrap(object *ob, AssetWrapper *parent) { return myWrappedObjects.wrap(ob, parent, this); }
        AssetWrapper *wrap(Face *face, AssetWrapper *parent) { return myWrappedFaces.wrap(face, parent, this); }
        AssetWrapper *wrap(Animations *anim, AssetWrapper *parent) { return myWrappedAnimations.wrap(anim, parent, this); }
        AssetWrapper *wrap(treasurelist *tr, AssetWrapper *parent) { return myWrappedTreasureLists.wrap(tr, parent, this); }
        AssetWrapper *wrap(treasure *tr, AssetWrapper *parent) { return myWrappedTreasures.wrap(tr, parent, this); }
        AssetWrapper *wrap(region *reg, AssetWrapper *parent) { return myWrappedRegions.wrap(reg, parent, this); }
        AssetWrapper *wrap(face_sets *fs, AssetWrapper *parent) { return myWrappedFacesets.wrap(fs, parent, this); }
        AssetWrapper *wrap(GeneralMessage *message, AssetWrapper *parent) { return myWrappedMessages.wrap(message, parent, this); }
        AssetWrapper *wrap(quest_definition *quest, AssetWrapper *parent) { return myWrappedQuests.wrap(quest, parent, this); }
        AssetWrapper *wrap(artifactlist *al, AssetWrapper *parent) { return myWrappedArtifactLists.wrap(al, parent, this); }
        AssetWrapper *wrap(artifact *art, AssetWrapper *parent) { return myWrappedArtifacts.wrap(art, parent, this); }
        AssetWrapper *wrap(recipelist *list, AssetWrapper *parent) { return myWrappedRecipeLists.wrap(list, parent, this); }
        AssetWrapper *wrap(recipe *rc, AssetWrapper *parent) { return myWrappedRecipes.wrap(rc, parent, this); }

    public slots:
      void archetypeModified(archetype *arch);
      void saveArchetypes();
      void questModified(quest_definition *quest);
      void saveQuests();
      void treasureModified(treasurelist *treasure);
      void saveTreasures();
      void generalMessageModified(GeneralMessage *message);
      void saveGeneralMessages();

    protected:
        CREMapInformationManager *myMapInformationManager;
        QList<QHash<QString, recipestruct*> > myRecipes;
        ModifiedAssetsManager<archetype> myArchetypes;
        ModifiedAssetsManager<quest_definition> myQuests;
        ModifiedAssetsManager<treasurelist> myTreasures;
        ModifiedAssetsManager<GeneralMessage> myGeneralMessages;
        AssetWrapperManager<archt, ArchetypeWrapper> myWrappedArchetypes;
        AssetWrapperManager<object, ObjectWrapper> myWrappedObjects;
        AssetWrapperManager<Face, FaceWrapper> myWrappedFaces;
        AssetWrapperManager<Animations, AnimationWrapper> myWrappedAnimations;
        AssetWrapperManager<treasurelist, TreasureListWrapper> myWrappedTreasureLists;
        AssetWrapperManager<treasure, TreasureWrapper> myWrappedTreasures;
        AssetWrapperManager<region, RegionWrapper> myWrappedRegions;
        AssetWrapperManager<face_sets, FacesetsWrapper> myWrappedFacesets;
        AssetWrapperManager<GeneralMessage, GeneralMessageWrapper> myWrappedMessages;
        AssetWrapperManager<quest_definition, QuestWrapper> myWrappedQuests;
        AssetWrapperManager<artifactlist, ArtifactListWrapper> myWrappedArtifactLists;
        AssetWrapperManager<artifact, ArtifactWrapper> myWrappedArtifacts;
        AssetWrapperManager<recipelist, RecipeListWrapper> myWrappedRecipeLists;
        AssetWrapperManager<recipe, RecipeWrapper> myWrappedRecipes;
};

#endif /* RESOURCESMANAGER_H */
