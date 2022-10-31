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
#include "attack_messages/AttackMessagesWrapper.h"
#include "treasures/TreasuresWrapper.h"
#include "sounds/SoundFiles.h"
#include "sounds/GameSounds.h"

AllAssets::AllAssets(ResourcesManager *resources, ScriptFileManager *scripts, MessageManager *messages) : AssetWrapper(nullptr, QString()) {

    myAssets.append(myRegions = new RegionsWrapper(this, resources));
    myAssets.append(new ArchetypesWrapper(this, resources));
    myAssets.append(new FacesWrapper(this, resources));
    myAssets.append(new AnimationsWrapper(this, resources));
    myAssets.append(new TreasuresWrapper(this, resources));
    myAssets.append(new AssetsCollectionWrapper<face_sets>(this, tr("Facesets"), getManager()->facesets(), resources, tr("Display all facesets.")));
    myAssets.append(new GeneralMessagesWrapper(this, resources));
    myAssets.append(new QuestsWrapper(this, resources));
    myAssets.append(new ArtifactsWrapper(this, resources));
    myAssets.append(new RecipesWrapper(this, resources));
    myAssets.append(scripts);
    myAssets.append(messages);
    myAssets.append(myRandomMaps = new RandomMaps(this, resources->getMapInformationManager()));
    myAssets.append(new AttackMessagesWrapper(this));
    myAssets.append(mySounds = new SoundFiles(this));
    myAssets.append(new GameSounds(this));
}
