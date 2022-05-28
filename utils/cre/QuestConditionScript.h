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

#ifndef _QUESTCONDITIONSCRIPT_H
#define _QUESTCONDITIONSCRIPT_H

#include <QObject>

/**
 * One pre- or post- condition Python script which may be used in a NPC dialog.
 */
class QuestConditionScript : public QObject
{
    Q_OBJECT

    public:
        QuestConditionScript(const QString& name, const QString& comment);
        virtual ~QuestConditionScript();

        const QString& name() const;
        const QString& comment() const;

    private:
        QString myName;
        QString myComment;
};

#endif /* _QUESTCONDITIONSCRIPT_H */
