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

#include "AssetModel.h"
#include "AssetWrapper.h"
#include <QScriptEngine>
#include <QMimeData>

AssetModel::AssetModel(AssetWrapper *assets, QObject *parent) : QAbstractItemModel(parent) {
    myAssets = assets;
    connect(myAssets, SIGNAL(dataModified(AssetWrapper *, AssetWrapper::ChangeType, int)), this, SLOT(assetModified(AssetWrapper *, AssetWrapper::ChangeType, int)));
}

AssetModel::~AssetModel() {
}

int AssetModel::columnCount(const QModelIndex &) const {
    return 1;
}

QModelIndex AssetModel::index(int row, int column, const QModelIndex& parent) const {
    if (column != 0) {
        return QModelIndex();
    }

    if (parent.isValid()) {
        AssetWrapper *wrapper = static_cast<AssetWrapper *>(parent.internalPointer());
        return createIndex(row, column, wrapper->child(row));
    }

    if (row < 0 || row >= myAssets->childrenCount()) {
        return QModelIndex();
    }
    auto child = myAssets->child(row);
    return createIndex(row, column, child);
}

QModelIndex AssetModel::parent(const QModelIndex& index) const {
    if (!index.isValid() || index.column() != 0) {
        return QModelIndex();
    }

    auto item = static_cast<AssetWrapper *>(index.internalPointer());
    if (!item || !item->displayParent()) {
        return QModelIndex();
    }
    auto parent = item->displayParent();
    if (!parent->displayParent()) {
        return QModelIndex();
    }
    return createIndex(parent->displayParent()->childIndex(parent), 0, parent);
}

int AssetModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return static_cast<AssetWrapper *>(parent.internalPointer())->childrenCount();
    }
    return myAssets->childrenCount();
}

QVariant AssetModel::data(const QModelIndex& index, int role) const {
    if ((Qt::DisplayRole != role && Qt::DecorationRole != role) || !index.isValid() || !index.internalPointer()) {
        return QVariant();
    }

    auto wrapper = static_cast<AssetWrapper *>(index.internalPointer());
    if (Qt::DecorationRole == role) {
        return wrapper->displayIcon();
    }
    return wrapper->displayName();
}

QVariant AssetModel::headerData(int section, Qt::Orientation orientation, int role) const {
    (void)orientation;
    (void)role;
    switch(section)
    {
        case 0:
            return tr("Asset");
    }

    return QVariant();
}

Qt::ItemFlags AssetModel::flags(const QModelIndex &index) const {
    auto flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
    if (index.isValid() && index.internalPointer()) {
        auto wrapper = static_cast<AssetWrapper *>(index.internalPointer());
        if (wrapper->canDrag()) {
            flags |= Qt::ItemIsDragEnabled;
        }
    }
    return flags;
}

QMimeData *AssetModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *data = nullptr;
    for (auto index : indexes) {
        if (index.isValid() && index.internalPointer()) {
            auto wrapper = static_cast<AssetWrapper *>(index.internalPointer());
            if (wrapper->canDrag()) {
                if (!data) {
                    data = new QMimeData();
                }
                wrapper->drag(data);
            }
        }
    }
    return data;
}

bool AssetModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const {
    (void)column;
    if (action != Qt::CopyAction) {
        return false;
    }

    if (parent.isValid() && parent.internalPointer()) {
        auto wrapper = static_cast<AssetWrapper *>(parent.internalPointer());
        return wrapper->canDrop(data, row);
    }
    return false;
}

bool AssetModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    (void)column;
    if (action != Qt::CopyAction) {
        return false;
    }

    if (parent.isValid() && parent.internalPointer()) {
        auto wrapper = static_cast<AssetWrapper *>(parent.internalPointer());
        wrapper->drop(data, row);
        return true;
    }
    return false;
}

void AssetModel::assetModified(AssetWrapper *asset, AssetWrapper::ChangeType type, int extra) {
    auto parent = asset->displayParent();
    if (!parent) {
        emit dataChanged(QModelIndex(), QModelIndex());
        return;
    }

    auto idx = createIndex(parent->childIndex(asset), 0, asset);
    switch (type) {
        case AssetWrapper::AssetUpdated: {
            emit dataChanged(idx, idx);
            if (extra > 0) {
                int count = asset->childrenCount() - 1;
                auto left = createIndex(0, 0, asset->child(0));
                auto right = createIndex(count, 0, asset->child(count));
                emit dataChanged(left, right);
            }
            break;
        }
        case AssetWrapper::BeforeChildAdd: {
            beginInsertRows(idx, extra, extra);
            break;
        }
        case AssetWrapper::AfterChildAdd: {
            endInsertRows();
            break;
        }
        case AssetWrapper::BeforeChildRemove: {
            beginRemoveRows(idx, extra, extra);
            break;
        }
        case AssetWrapper::AfterChildRemove: {
            endRemoveRows();
            break;
        }
        case AssetWrapper::BeforeLayoutChange: {
            emit layoutAboutToBeChanged();
            break;
        }
        case AssetWrapper::AfterLayoutChange: {
            emit layoutChanged();
            break;
        }
    }
}

UseFilterAssetModel::UseFilterAssetModel(QObject *parent) : QSortFilterProxyModel(parent), myAsset(nullptr) {
};

void UseFilterAssetModel::setFilter(AssetWrapper *asset) {
    myAsset = asset;
    myCachedFilter.clear();
    myCachedHints.clear();
    invalidateFilter();
}

bool UseFilterAssetModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (!myAsset) {
        return true;
    }

    auto index = sourceModel()->index(sourceRow, 0, sourceParent);
    auto wrapper = qobject_cast<AssetWrapper *>(static_cast<QObject *>(index.internalPointer()));
    if (!wrapper) {
        return false;
    }

    auto cache = myCachedFilter.find(wrapper);
    if (cache != myCachedFilter.end()) {
        return cache->second;
    }

    bool uses = false;
    std::string hint;
    auto possibleUse = wrapper->uses(myAsset, hint);
    if (AssetWrapper::Uses == possibleUse) {
        uses = true;
        if (!hint.empty()) {
            myCachedHints[wrapper] = hint;
        }
    } else if (AssetWrapper::ChildrenMayUse == possibleUse) {
        int count = sourceModel()->rowCount(index);
        for (int c = 0; c < count && !uses; c++) {
            uses |= filterAcceptsRow(c, index);
        }
    }
    myCachedFilter[wrapper] = uses;
    return uses;
}

QVariant UseFilterAssetModel::data(const QModelIndex& index, int role) const {
    QVariant ret = QSortFilterProxyModel::data(index, role);
    if (role == Qt::DisplayRole) {
        auto sourceIndex = mapToSource(index);
        auto w = qobject_cast<AssetWrapper *>(static_cast<QObject *>(sourceIndex.internalPointer()));
        auto hint = w == nullptr ? myCachedHints.end() : myCachedHints.find(w);
        if (hint != myCachedHints.end()) {
            return ret.toString() + QString(" (") + QString::fromLocal8Bit(hint->second.c_str()) + ")";
        }
    }
    return ret;
}

ScriptFilterAssetModel::ScriptFilterAssetModel(AssetModel *model, QScriptEngine *engine, QObject *parent)
  : QSortFilterProxyModel(parent), myEngine(engine) {
    setSourceModel(model);
}

void ScriptFilterAssetModel::setFilter(const QString &filter) {
    myFilter = filter;
    myCachedFilter.clear();
    invalidateFilter();
}

bool ScriptFilterAssetModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (myFilter.isEmpty())
        return true;

    auto index = sourceModel()->index(sourceRow, 0, sourceParent);
    auto wrapper = qobject_cast<AssetWrapper *>(static_cast<QObject *>(index.internalPointer()));
    if (!wrapper) {
        return false;
    }

    auto cache = myCachedFilter.find(wrapper);
    if (cache != myCachedFilter.end()) {
        return cache->second;
    }

    bool accept = acceptItem(wrapper);
    if (!accept) {
        int count = sourceModel()->rowCount(index);
        for (int c = 0; c < count && !accept; c++) {
            accept |= filterAcceptsRow(c, index);
        }
    }
    myCachedFilter[wrapper] = accept;
    return accept;
}

bool ScriptFilterAssetModel::acceptItem(AssetWrapper *item) const {
    QScriptValue engineValue = myEngine->newQObject(item);
    myEngine->globalObject().setProperty("item", engineValue);

    myEngine->pushContext();
    bool show = myEngine->evaluate(myFilter).toBoolean();
    myEngine->popContext();
    if (myEngine->hasUncaughtException()) {
        //qDebug() << myEngine->uncaughtException().toString();
        return false;
    }

    return show;
}
