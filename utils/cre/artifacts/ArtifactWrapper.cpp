#include "ArtifactWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "faces/FaceWrapper.h"
#include "ArtifactListWrapper.h"

ArtifactWrapper::ArtifactWrapper(AssetWrapper *parent, const artifact *art, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "Artifact", art), myResourcesManager(resourcesManager)
{
}

QString ArtifactWrapper::displayName() const {
    QString name(myItem->item->name);
    auto list = find_artifactlist(myItem->item->type);
    if (list && list->total_chance != 0) {
        name = tr("%1 (%2%, %3 chances on %4)")
            .arg(name)
            .arg(qRound((float)100 * myItem->chance / list->total_chance))
            .arg(myItem->chance)
            .arg(list->total_chance);
    }

    return name;
}

QIcon ArtifactWrapper::displayIcon() const {
    return CREPixmap::getIcon(artifact_get_face(myItem));
}

QObject* ArtifactWrapper::item() {
    return myResourcesManager->wrap(myItem->item, this);
}

int ArtifactWrapper::chance() const {
    return myItem->chance;
}

int ArtifactWrapper::difficulty() const {
    return myItem->difficulty;
}

QStringList ArtifactWrapper::allowed() const {
    QStringList allowed;
    linked_char* a = myItem->allowed;

    while (a) {
        allowed.append(a->name);
        a = a->next;
    }
    return allowed;
}

AssetWrapper::PossibleUse ArtifactWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) { /** @todo handle face_suffix */
        return myItem->item->face == face->item() ? Uses : DoesntUse;
    }
    auto anim = dynamic_cast<const AnimationWrapper *>(asset);
    if (anim) { /** @todo handle anim_suffix */
        return myItem->item->animation == anim->item() ? Uses : DoesntUse;
    }
    return DoesntUse;
}
