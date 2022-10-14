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

#include <assert.h>
#include "QuestWrapper.h"
#include "faces/FaceWrapper.h"
#include "ResourcesManager.h"
#include "MimeUtils.h"

void QuestStepWrapper::fillMenu(QMenu *menu) {
    connect(menu->addAction(tr("Delete")), &QAction::triggered, [this] () { myParent->removeChild(this); });
}

void QuestStepWrapper::drag(QMimeData *data) const {
    QuestWrapper *qw = dynamic_cast<QuestWrapper *>(myParent);
    if (qw) {
        MimeUtils::addQuestStep(data, qw->code(), qw->childIndex(const_cast<QuestStepWrapper *>(this)));
    }
}

bool QuestStepWrapper::canDrop(const QMimeData *data, int) const {
    return data->hasFormat(MimeUtils::QuestStep);
}

void QuestStepWrapper::drop(const QMimeData *data, int row) {
    myParent->drop(data, myParent->childIndex(this) + (row == -1 ? 0 : 1));
}

QList<QStringList> QuestStepWrapper::conditions() const {
    QList<QStringList> ret;
    char buf[200];
    for (const auto condition : myWrappedItem->conditions) {
        quest_write_condition(buf, sizeof(buf), condition);
        ret.append(QString(buf).split(' '));
        Q_ASSERT(ret.back().size() == 2);
    }
    return ret;
}

void QuestStepWrapper::setConditions(const QList<QStringList> &conditions) {
    for (const auto cond : myWrappedItem->conditions) {
        quest_destroy_condition(cond);
    }
    myWrappedItem->conditions.clear();

    for (auto single : conditions) {
        auto cond = quest_create_condition();
        if (!quest_condition_from_string(cond, single.join(' ').toStdString().data())) {
            free(cond);
            continue;
        }
        myWrappedItem->conditions.push_back(cond);
    }
    markModified(AssetUpdated);
}

AssetWrapper::PossibleUse QuestWrapper::uses(const AssetWrapper *asset, std::string &) const {
    auto face = dynamic_cast<const FaceWrapper *>(asset);
    if (face) {
        return myWrappedItem->face == face->wrappedItem() ? Uses : DoesntUse;
    }
    return DoesntUse;
}

int QuestWrapper::childrenCount() const {
    return myWrappedItem->steps.size();
}

AssetWrapper *QuestWrapper::child(int index) {
    if (index >= 0 && index < static_cast<int>(myWrappedItem->steps.size())) {
        return myResources->wrap(myWrappedItem->steps[index], this);
    }
    return nullptr;
}

int QuestWrapper::childIndex(AssetWrapper *child) {
    for (int index = 0; index < static_cast<int>(myWrappedItem->steps.size()); index++) {
        if (child == myResources->wrap(myWrappedItem->steps[index], this)) {
            return index;
        }
    }
    return -1;
}

void QuestWrapper::wasModified(AssetWrapper *asset, ChangeType type, int extra) {
    myResources->questModified(myWrappedItem);
    AssetTWrapper::wasModified(asset, type, extra);
}

bool QuestWrapper::canDrop(const QMimeData *data, int) const {
    return data->hasFormat(MimeUtils::QuestStep);
}

void QuestWrapper::drop(const QMimeData *data, int row) {
    std::vector<quest_step_definition *> &steps(myWrappedItem->steps);
    if (steps.size() < 2) {
        return;
    }

    if (row == -1) {
        row = 0;
    }

    auto dropped = MimeUtils::extractQuestSteps(data);
    for (auto single : dropped) {
        if (single.first != code()) {
            continue;
        }

        if (single.second < 0 || single.second >= static_cast<int>(steps.size()) || single.second == row) {
            continue;
        }

        markModified(BeforeChildRemove, single.second);
        auto step = steps[single.second];
        steps.erase(steps.begin() + single.second);
        markModified(AfterChildRemove, single.second);
        if (row > single.second) {
            row--;
        }

        markModified(BeforeChildAdd, row);
        steps.insert(steps.begin() + row, step);
        markModified(AfterChildAdd, row);
    }
}

void QuestWrapper::fillMenu(QMenu *menu) {
    if (myParent) {
        myParent->fillMenu(menu);
    }
    connect(menu->addAction(tr("Add step")), &QAction::triggered, [this] () { addStep(); });
}

void QuestWrapper::addStep() {
    auto max = std::max_element(myWrappedItem->steps.cbegin(), myWrappedItem->steps.cend(),
            [] (const auto &left, const auto &right) { return left->step < right->step; });
    auto ns = quest_create_step();
    ns->step = (max == myWrappedItem->steps.end() ? 0 : (*max)->step) + 10;
    markModified(BeforeChildAdd, myWrappedItem->steps.size());
    myWrappedItem->steps.push_back(ns);
    markModified(AfterChildAdd, myWrappedItem->steps.size());
}

void QuestWrapper::removeChild(AssetWrapper *child) {
    int index = childIndex(child);
    if (index == -1) {
        return;
    }
    markModified(BeforeChildRemove, index);
    auto step = myWrappedItem->steps[index];
    myWrappedItem->steps.erase(myWrappedItem->steps.begin() + index);
    quest_destroy_step(step);
    markModified(AfterChildRemove, index);
}
