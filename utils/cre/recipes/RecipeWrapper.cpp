#include "RecipeWrapper.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"

RecipeWrapper::RecipeWrapper(AssetWrapper *parent, const recipe *rp, ResourcesManager *)
  : AssetTWrapper(parent, "Recipe", rp) {
}

QString RecipeWrapper::title() const
{
    return myItem->title;
}

int RecipeWrapper::chance() const
{
    return myItem->chance;
}

int RecipeWrapper::difficulty() const
{
    return myItem->diff;
}

int RecipeWrapper::experience() const
{
    return myItem->exp;
}

QStringList RecipeWrapper::archs() const
{
    QStringList archs;
    for (size_t i = 0; i < myItem->arch_names; i++)
    {
        archs.append(myItem->arch_name[i]);
    }
    return archs;
}

QStringList RecipeWrapper::ingredients() const
{
    QStringList ingredients;
    for (linked_char* ing = myItem->ingred; ing; ing = ing->next)
    {
        ingredients.append(ing->name);
    }
    return ingredients;
}

bool RecipeWrapper::transmute() const
{
    return myItem->transmute != 0;
}

int RecipeWrapper::minLevel() const
{
    return myItem->min_level;
}

QString RecipeWrapper::keycode() const
{
    return myItem->keycode;
}

QString RecipeWrapper::displayName() const {
    if (myItem->arch_names == 0) {
        return QString("%1 (no archetype?)").arg(myItem->title);
    }
    auto base = find_archetype(myItem->arch_name[0]);
    if (!base) {
        return QString("%1 (no archetype?)").arg(myItem->title);
    }

    if (strcmp(myItem->title, "NONE") == 0) {
        if (base->clone.title) {
            return QString("%1 %2").arg(base->clone.name, base->clone.title);
        }
        return base->clone.name;
    }
    return QString("%1 of %2").arg(base->clone.name, myItem->title);
}

QIcon RecipeWrapper::displayIcon() const {
    const Face *face = recipe_get_face(myItem);
    const archt* base = NULL;
    if (myItem->arch_names > 0) {
        base = find_archetype(myItem->arch_name[0]);
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
