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

#ifndef ARTIFACT_LOADER_H
#define ARTIFACT_LOADER_H

#include "AssetLoader.h"

class ArtifactLoader : public AssetLoader {

public:
  ArtifactLoader(AssetsTracker *tracker) : myTracker(tracker) { }
  virtual bool willLoad(const std::string &filename) override;

  virtual void load(BufferReader *reader, const std::string &filename) override;

private:
  AssetsTracker *myTracker;
};

#endif /* ARTIFACT_LOADER_H */
