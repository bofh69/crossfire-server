#include "ArchetypesWrapper.h"
#include "../ResourcesManager.h"

#include "assets.h"
#include "AssetsManager.h"

ArchetypesWrapper::ArchetypesWrapper(AssetWrapper *parent, ResourcesManager *resourcesManager)
 : AssetWrapper(parent), myResourcesManager(resourcesManager) {
    getManager()->archetypes()->each([&] (archt *arch) {
        if (!arch->head) {
            myArch.append(myResourcesManager->wrap(arch, this));
        }
    });
    qSort(myArch.begin(), myArch.end(), compareByDisplayName);
    setProperty(tipProperty, tr("Display all archetypes."));
}
