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
    auto condition = myWrappedItem->conditions;
    while (condition) {
        quest_write_condition(buf, sizeof(buf), condition);
        ret.append(QString(buf).split(' '));
        Q_ASSERT(ret.back().size() == 2);
        condition = condition->next;
    }
    return ret;
}

void QuestStepWrapper::setConditions(const QList<QStringList> &conditions) {
    auto cond = myWrappedItem->conditions;
    while (cond) {
        auto n = cond->next;
        quest_destroy_condition(cond);
        cond = n;
    }
    myWrappedItem->conditions = nullptr;
    quest_condition *last = nullptr;

    for (auto single : conditions) {
        auto cond = quest_create_condition();
        if (!quest_condition_from_string(cond, single.join(' ').toStdString().data())) {
            free(cond);
            continue;
        }
        if (last)
            last->next = cond;
        else
            myWrappedItem->conditions = cond;
        last = cond;
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
    int count = 0;
    auto step = myWrappedItem->steps;
    while (step) {
        step = step->next;
        count++;
    }
    return count;
}

AssetWrapper *QuestWrapper::child(int index) {
    auto step = myWrappedItem->steps;
    while (step && index > 0) {
        step = step->next;
        index--;
    }
    return step ? myResources->wrap(step, this) : nullptr;
}

int QuestWrapper::childIndex(AssetWrapper *child) {
    int index = 0;
    auto step = myWrappedItem->steps;
    while (step && myResources->wrap(step, this) != child) {
        index++;
        step = step->next;
    }
    return step == nullptr ? -1 : index;
}

void QuestWrapper::wasModified(AssetWrapper *asset, ChangeType type, int extra) {
    myResources->questModified(myWrappedItem);
    AssetTWrapper::wasModified(asset, type, extra);
}

bool QuestWrapper::canDrop(const QMimeData *data, int) const {
    return data->hasFormat(MimeUtils::QuestStep);
}

void QuestWrapper::drop(const QMimeData *data, int row) {
    std::vector<quest_step_definition *> steps;
    auto pos = myWrappedItem->steps;
    while (pos) {
        steps.push_back(pos);
        pos = pos->next;
    }
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

    pos = myWrappedItem->steps = steps[0];
    auto step = steps.begin();
    ++step;
    while (step < steps.end()) {
        pos->next = (*step);
        pos = pos->next;
        pos->next = nullptr;
        ++step;
    }
}

void QuestWrapper::fillMenu(QMenu *menu) {
    if (myParent) {
        myParent->fillMenu(menu);
    }
    connect(menu->addAction(tr("Add step")), &QAction::triggered, [this] () { addStep(); });
}

void QuestWrapper::addStep() {
    int index = 0, count = 0;;
    quest_step_definition *step = myWrappedItem->steps, *last = nullptr;
    while (step) {
        index = qMax(index, step->step);
        last = step;
        step = step->next;
        count++;
    }
    markModified(BeforeChildAdd, count);
    auto ns = quest_create_step();
    ns->step = index + 10;
    if (last) {
        last->next = ns;
    } else {
        assert(!myWrappedItem->steps);
        myWrappedItem->steps = ns;
    }
    markModified(AfterChildAdd, count);
}

void QuestWrapper::removeChild(AssetWrapper *child) {
    quest_step_definition *step = myWrappedItem->steps, *last = nullptr;
    int index = 0;
    while (myResources->wrap(step, this) != child) {
        index++;
        last = step;
        step = step->next;
    }

    if (!step) {
        return;
    }

    markModified(BeforeChildRemove, index);
    if (last == nullptr) {
        assert(step == myWrappedItem->steps);
        myWrappedItem->steps = step->next;
    } else {
        last->next = step->next;
    }
    quest_destroy_step(step);
    markModified(AfterChildRemove, index);
}
