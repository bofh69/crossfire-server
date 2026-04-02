# Welcome to Crossfire Resource Editor
This tool helps browse and (sometimes) edit Crossfire resources, including:

 * [treasures](treasures.html)
 * archetypes
 * general messages
 * NPC dialogs
 * [quests](quests.html)
 * artifacts
 * alchemy recipes
 * and many others

This tool does **not** allow map edition, which is done through Gridarta, available from
http://crossfire.real-time.com/editors/install/windows7-sun_v6.html

## Notes

 * knowledge of Crossfire as a player is a must-have, and technical knowledge is useful
 * Crossfire `resources` and `assets` are the same thing, it's just a vocabulary change
 * this tool is still experimental, it may crash at any time. Save your changes often!

# Basics
The most useful window to interact with resources is the `asset` window, opened by selecting
what to display in the `Open` menu. You can open as many windows as you want.

The asset window has 2 panels:

 * the `asset tree`, on the left, lists all assets of the selected type, or all available assets. Assets
are displayed as a tree, with children you can expand
 * the `asset panel`, on the right, shows details of the selected item in the asset tree. This is where
you can edit assets.

From the asset tree, you can sometimes drag an asset to other parts of CRE, or drop other assets.

Double-clicking in a grid allows edition of the item, provided it can be edited.

If a zone is not read-only, then it can be edited.

On an archetype list in a panel, an archetype may be dragged and dropped from the asset tree.

**Note**: `experience` is not considered an asset, and has its own different window.

# Types of assets

Here is a quick summary of the different Crossfire assets you can access with CRE:

 * **archetypes** represent templates for all items, monsters, spells, skills, ...
 * **artifacts** are modifiers to archetypes, changing values and adding a title, specific
properties. They are randomly applied to archetypes when generating treasures
 * **faces** are visuals shown to the player, graphics and such
 * **animations** are groups of faces, and are used to show facing directions, animate
monsters or items or such
 * [**treasures**](treasures.html) give equipment to monsters, loot to chests, etc.
 * **facesets** are groups of faces the player can select, to change the game's theme
 * **general messages** are messages appearing in books, scrolls, various readings
 * [**quests**](quests.html) can be completed by players for rewards or to gain access to places
 * **scripts** are used to customize some actions
 * **NPC dialogs** allow NPCs to have rich dialogs, with options depending on quests
the player completed, items in inventory, and other things
 * **random maps** are used as intermediary maps or standalone maps

# Changes

Major changes tp CRE are listed in [changes](changes.html).
