# Quests

## Basics

A `quest` is a player's accomplishment, which may be done only once or multiple times,
depending on the quest.

A quest is composed of different `steps`, each one having:

 * `an internal code`, numeric value which must be unique for the quest
 * a `descriptive text`
 * whether this step ends the quest or not

## Editing

On the asset tree, the quests will be displayed with steps as their children. In the right panel, you can:

 * edit attributes of the quest (the `code` is read-only)
 * edit attributes of the quest step

You can also right-click on the asset tree to add a new step or remove steps. Drag and drop allows reordering of steps in a quest (you can't drag a step from a quest to another quest).
