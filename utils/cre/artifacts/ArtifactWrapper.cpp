#include "ArtifactWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "faces/FaceWrapper.h"
#include "ArtifactListWrapper.h"

ArtifactWrapper::ArtifactWrapper(AssetWrapper *parent, artifact *art, ResourcesManager *resourcesManager)
 : AssetTWrapper(parent, "Artifact", art), myResourcesManager(resourcesManager), mySpecificItem(nullptr)
{
}

QString ArtifactWrapper::displayName() const {
    QString name(myWrappedItem->item->name);
    if (mySpecificItem) {
        int n, d;
        artifact_compute_chance_for_item(mySpecificItem, myWrappedItem, &n, &d);
        name = tr("%1 (%2%, %3 chances on %4)")
                .arg(name)
                .arg(qRound(100. * n / d))
                .arg(n)
                .arg(d);
    } else {
        auto list = find_artifactlist(myWrappedItem->item->type);
        if (list && list->total_chance != 0) {
            name = tr("%1 (%2%, %3 chances on %4)")
                .arg(name)
                .arg(qRound((float)100 * myWrappedItem->chance / list->total_chance))
                .arg(myWrappedItem->chance)
                .arg(list->total_chance);
        }
    }

    return name;
}

QIcon ArtifactWrapper::displayIcon() const {
    return CREPixmap::getIcon(artifact_get_face(myWrappedItem));
}

QString ArtifactWrapper::name() const {
    return myWrappedItem->item->name;
}

QObject* ArtifactWrapper::item() {
    return myResourcesManager->wrap(myWrappedItem->item, this);
}

int ArtifactWrapper::chance() const {
    return myWrappedItem->chance;
}

int ArtifactWrapper::difficulty() const {
    return myWrappedItem->difficulty;
}

QStringList ArtifactWrapper::allowed() const {
    QStringList allowed;
    linked_char* a = myWrappedItem->allowed;

    while (a) {
        allowed.append(a->name);
        a = a->next;
    }
    return allowed;
}

AssetWrapper::PossibleUse ArtifactWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) { /** @todo handle face_suffix */
        return myWrappedItem->item->face == face->wrappedItem() ? Uses : DoesntUse;
    }
    auto anim = dynamic_cast<const AnimationWrapper *>(asset);
    if (anim) { /** @todo handle anim_suffix */
        return myWrappedItem->item->animation == anim->wrappedItem() ? Uses : DoesntUse;
    }
    return DoesntUse;
}
