#include "AllAssets.h"
#include "ResourcesManager.h"
#include "CREMapInformationManager.h"

#include "assets.h"
#include "AssetsManager.h"
#include "artifacts/ArtifactWrapper.h"
#include "recipes/RecipeWrapper.h"
#include "treasures/TreasureListWrapper.h"
#include "regions/RegionsWrapper.h"
#include "archetypes/ArchetypesWrapper.h"
#include "assets/AssetsCollectionWrapper.h"
#include "artifacts/ArtifactsWrapper.h"
#include "recipes/RecipesWrapper.h"
#include "scripts/ScriptFileManager.h"
#include "MessageManager.h"
#include "random_maps/RandomMaps.h"
#include "faces/FacesWrapper.h"
#include "animations/AnimationsWrapper.h"
#include "quests/QuestsWrapper.h"
#include "general_messages/GeneralMessagesWrapper.h"

AllAssets::AllAssets(ResourcesManager *resources, ScriptFileManager *scripts, MessageManager *messages) : AssetWrapper(nullptr, QString()) {

    myAssets.append(myRegions = new RegionsWrapper(this, resources));
    myAssets.append(new ArchetypesWrapper(this, resources));
    myAssets.append(new FacesWrapper(this, resources));
    myAssets.append(new AnimationsWrapper(this, resources));
    myAssets.append(new AssetsCollectionWrapper<treasurelist>(this, "Treasures", getManager()->treasures(), resources, tr("Display all treasure lists.")));
    myAssets.append(new AssetsCollectionWrapper<face_sets>(this, "Facesets", getManager()->facesets(), resources, tr("Display all facesets.")));
    myAssets.append(new GeneralMessagesWrapper(this, resources));
    myAssets.append(new QuestsWrapper(this, resources));
    myAssets.append(new ArtifactsWrapper(this, resources));
    myAssets.append(new RecipesWrapper(this, resources));
    myAssets.append(scripts);
    myAssets.append(messages);
    myAssets.append(myRandomMaps = new RandomMaps(this, resources->getMapInformationManager()));
}
