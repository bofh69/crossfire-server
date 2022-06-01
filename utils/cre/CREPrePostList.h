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

#ifndef CREPREPOSTLIST_H
#define CREPREPOSTLIST_H

#include <QDialog>

class QListWidget;
class MessageManager;

class PrePostWidget : public QWidget {
    Q_OBJECT

    Q_PROPERTY(QList<QStringList> data READ data WRITE setData)

public:
    enum Mode { PreConditions, PostConditions, SetWhen };

    PrePostWidget(QWidget* parent, Mode mode, const MessageManager* manager);

    QList<QStringList> data() const;
    void setData(const QList<QStringList>& data);

signals:
    void dataModified();

private slots:
    void onAddCondition(bool);
    void onDeleteCondition(bool);
    void onReset(bool);

private:
    void addItem(const QStringList& item);
    QListWidget* myList;
    QList<QStringList> myOriginal;
    Mode myMode;
};

/**
 * Display and allow edition of a list of pre- or post- conditions for a NPC message,
 * or quest states for a quest step.
 */
class CREPrePostList : public QDialog
{
    Q_OBJECT

public:
    CREPrePostList(QWidget* parent, PrePostWidget::Mode mode, const MessageManager* manager);

    QList<QStringList> data() const { return myWidget->data(); }
    void setData(const QList<QStringList>& data) { myWidget->setData(data); }

private:
    PrePostWidget *myWidget;
};

#endif /* CREPREPOSTLIST_H */

