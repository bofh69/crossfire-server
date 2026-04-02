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

#ifndef _CRESTRINGLISTPANEL_H
#define _CRESTRINGLISTPANEL_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;

/**
 * Display and allow edition of a list of multiline strings.
 */
class CREStringListPanel : public QDialog
{
    Q_OBJECT

    public:
        CREStringListPanel(QWidget* parent);
        virtual ~CREStringListPanel();

        void setData(const QStringList& list);
        QStringList data() const;

    private:
        QListWidgetItem* createItem(const QString& text);

        QListWidget* myItems;
        QStringList myOriginal;

    protected slots:
        void onAddItem(bool);
        void onDeleteItem(bool);
        void onReset(bool);
};

#endif /* _CRESTRINGLISTPANEL_H */
