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

#include "AssetFieldCompleter.h"
#include <QStandardItemModel>
#include "archetypes/ArchetypeWrapper.h"
#include "regions/RegionWrapper.h"
#include "CREMapInformation.h"
#include "archetypes/ObjectWrapper.h"
#include "artifacts/ArtifactWrapper.h"
#include "treasures/TreasureListWrapper.h"
#include "faces/FaceWrapper.h"
#include "quests/QuestWrapper.h"

/** Structure to give a type name to available properties. */
struct AssetData {
    const char *name;                 /**< User-friendly name. */
    const QMetaObject *meta;          /**< Associated object with its properties. */
    std::vector<AssetData> subfields; /**< Specific information for some properties. */
};

/** All available completions. */
const std::vector<AssetData> assetData = {
    { "region", &RegionWrapper::staticMetaObject, { } },
    { "map", &CREMapInformation::staticMetaObject, { } },
    { "archetype", &ArchetypeWrapper::staticMetaObject, { { "clone", &ObjectWrapper::staticMetaObject, { } } } },
    { "artifact", &ArtifactWrapper::staticMetaObject, { } },
    { "treasurelist", &TreasureListWrapper::staticMetaObject, { } },
    { "face", &FaceWrapper::staticMetaObject, { } },
    { "quest", &QuestWrapper::staticMetaObject, { } },
};

/**
 * Fill the model with properties for the assets.
 * @param root base item to add properties to.
 * @param data asset fields to add.
 */
static void addAssetData(QStandardItem *root, const AssetData &data) {
    std::map<QString, QStandardItem *> fields;
    for (int index = 0; index < data.meta->propertyCount(); ++index) {
        auto property = data.meta->property(index);
        if (fields.cend() == fields.find(property.name())) {
            fields[property.name()] = new QStandardItem(property.name());
            root->appendRow(fields[property.name()]);
        }
        for (const auto &sub : data.subfields) {
            addAssetData(fields[property.name()], sub);
        }
    }
}

AssetFieldCompleter::AssetFieldCompleter(QWidget *parent) : QCompleter(parent) {
    QStandardItemModel *model = new QStandardItemModel();
    auto root = model->invisibleRootItem();
    auto item = new QStandardItem("item");
    for (const auto &data : assetData) {
        addAssetData(item, data);
    }
    root->appendRow(item);

    setModel(model);
}

QStringList AssetFieldCompleter::splitPath(const QString &path) const {
    return path.split('.');
}

QString AssetFieldCompleter::pathFromIndex(const QModelIndex &index) const {
    QStringList dataList;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        dataList.prepend(model()->data(i, completionRole()).toString());
    }

    return dataList.join('.');
}
