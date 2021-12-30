# Treasures

## Basics

Treasures in Crossfire represent what items or skills a monster may have,
or contents of a chest, or items sold in a shop, or similar random items.

A `treasure list` consists of `treasure items`. The list groups all items which can be
chosen, with some properties.

A list can be `single item` or `multiple items`:

 * if single, only one item will be selected (this is for instance used in shops to chose one sold item).
In this case, an item's `chance` will represent the probability of this item being selected, relative to
the sum of all chances of other items.
 * if multiple, then all items will appear with a probability of their chance on 100 (this is used
to give monsters a weapon, a shield, some spells, and such).

A single treasure item can be either an archetype, or another list from which another
item would be chosen.

When using multiple items, an item can have a `yes` and/or `no` sub-item, that will be generated
if the item was, or not, generated. This allows giving arrows when a bow is chosen,
or giving a crossbow if a bow is not chosen.

## How to edit treasures

Editing treasure is done by selecting the list or item to edit in the resources tree:

 * a list can be toggled from single to multiple through the `Single item` checkbox
 * properties of items can be changed using the various fields
     * to select an archetype for an item, drag and drop an archetype to the item's panel
     * to select another list for an item, drag and drop a list (from another
treasure window, as clicking will change the displayed panel) to the item's panel
 * to add a yes or no sub-item to an item, drag and drop an archetype or treasure
list on this item. You can switch yes and no by right-clicking on the yes, no or the item with sub-items
 * to remove items or sub-items, right-click and select `delete`
 * note: you can not add or remove treasure lists for now
