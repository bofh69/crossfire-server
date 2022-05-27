#include "RecipeWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "../archetypes/ArchetypeWrapper.h"

RecipeWrapper::RecipeWrapper(AssetWrapper *parent, const recipe *rp, ResourcesManager *)
  : AssetTWrapper(parent, "Recipe", rp) {
}

QString RecipeWrapper::title() const
{
    return myWrappedItem->title;
}

int RecipeWrapper::chance() const
{
    return myWrappedItem->chance;
}

int RecipeWrapper::difficulty() const
{
    return myWrappedItem->diff;
}

int RecipeWrapper::experience() const
{
    return myWrappedItem->exp;
}

QStringList RecipeWrapper::archs() const
{
    QStringList archs;
    for (size_t i = 0; i < myWrappedItem->arch_names; i++)
    {
        archs.append(myWrappedItem->arch_name[i]);
    }
    return archs;
}

QStringList RecipeWrapper::ingredients() const
{
    QStringList ingredients;
    for (linked_char* ing = myWrappedItem->ingred; ing; ing = ing->next)
    {
        ingredients.append(ing->name);
    }
    return ingredients;
}

bool RecipeWrapper::transmute() const
{
    return myWrappedItem->transmute != 0;
}

int RecipeWrapper::minLevel() const
{
    return myWrappedItem->min_level;
}

QString RecipeWrapper::keycode() const
{
    return myWrappedItem->keycode;
}

QString RecipeWrapper::displayName() const {
    if (myWrappedItem->arch_names == 0) {
        return QString("%1 (no archetype?)").arg(myWrappedItem->title);
    }
    auto base = find_archetype(myWrappedItem->arch_name[0]);
    if (!base) {
        return QString("%1 (no archetype?)").arg(myWrappedItem->title);
    }

    if (strcmp(myWrappedItem->title, "NONE") == 0) {
        if (base->clone.title) {
            return QString("%1 %2").arg(base->clone.name, base->clone.title);
        }
        return base->clone.name;
    }
    return QString("%1 of %2").arg(base->clone.name, myWrappedItem->title);
}

QIcon RecipeWrapper::displayIcon() const {
    const Face *face = recipe_get_face(myWrappedItem);
    const archt* base = NULL;
    if (myWrappedItem->arch_names > 0) {
        base = find_archetype(myWrappedItem->arch_name[0]);
    }
    if (!face && base != NULL) {
        return CREPixmap::getIcon(base->clone.face);
    }
    return CREPixmap::getIcon(face);
}

void RecipeWrapper::displayFillPanel(QWidget *panel) {
    AssetWrapperPanel *p = static_cast<AssetWrapperPanel *>(panel);
    p->setItem(this);
}

AssetWrapper::PossibleUse RecipeWrapper::uses(const AssetWrapper *asset, std::string &hint) const {
    auto arch = dynamic_cast<const ArchetypeWrapper *>(asset);
    if (arch) {
        for (size_t i = 0; i < wrappedItem()->arch_names; i++) {
            if (strcmp(wrappedItem()->arch_name[i], arch->wrappedItem()->name) == 0) {
                hint = "alchemy product";
                return Uses;
            }
        }
        return DoesntUse;
    }
    return DoesntUse;
}
