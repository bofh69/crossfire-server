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

#ifndef QUESTCOMBOBOX_H
#define QUESTCOMBOBOX_H

#include <QtWidgets>

class quest_definition;

/**
 * Combobox displaying a list of quests, allowing selection and search.
 */
class QuestComboBox : public QComboBox {
    Q_OBJECT

    Q_PROPERTY(const quest_definition *quest READ quest WRITE setQuest)

 public:
    /**
     * Standard constructor.
     * @param parent ancestor of this widget.
     * @param allowNone whether to allow "(none)" face.
     */
    QuestComboBox(QWidget* parent, bool allowNone);
    virtual ~QuestComboBox();

    /**
     * Set the selected quest.
     * @param quest quest to select.
     */
    void setQuest(const quest_definition* quest);
    /**
     * Get the selected quest.
     * @return selected quest, nullptr if no face is selected.
     */
    const quest_definition* quest() const;
};

#endif /* FACECOMBOBOX_H */
