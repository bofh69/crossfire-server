# Alchemy {#system_alchemy}
This is a brief description of the ALCHEMY hack.

As always, the general intention is to enhance the enjoyability and
playability of CF. In this code, I have created a system whereby players may
assemble various ingredients to make artifacts.

The amount of work I put into this was substantial; other than new code -- I
created new archetypes, revised archetypes, altered the treasures file and
created 2 new object types and a flag.

It therefore may be enjoyable to install this patch (incl code) and *not*
define ALCHEMY in define.h. In particular, this patch does the following
things:

- more varieties of potions are available in shops.

- new gems and types of food.

- monsters will now leave behind body parts when you kill them.

- if ALCHEMY is defined players may make potions/artifacts.

I have included instructions for installation, an install script and a brief
document for using the code (pretty simple).

Finally, as always, feedback is appreciated.

b.t.  thomas@astro.psu.edu

## Player Instructions
Using the alchemy code (to make artifacts) is easy. You need the following
things: 1) be able to use the alchemy skill, 2) have access to a cauldron,
and 3) have the right ingredients.

To create an artifact, put ingredients in the cauldron, then use 'alchemy'
skill. You might make something :) Backfire effects are possible.
Books found in shops (and elsewhere) will give you formulae for making
stuff. Of course, you can always 'experiment' on your own, but this may be
dangerous.

## Implementation Details
The bulk fo the alchemy code resides in 2 files - recipe.c and alchemy.c.
The basic features of the code are:

- allow players to make artifacts from formulae. As players
gain xp and levels, their ability to create stuff improves

- fast lookup of formulae. Lookup is randomized so that the
list is attempted for a match based on # levels the player has. Obviously,
higher level players get more attempts to match the list--and thereby have a
grater chance of successfully completing a formula.

- There are ~80 formulae in this version. You can dump
the formulalists using the crossfire -m7 option.

Basically, a simple linked list, ordered by the number of different archs in
a formulae, is used to lookup 'recipe' matches. An indexed value for each
formulae allows a quick comparison for a match with the cauldron contents.
When a player casts alchemy on a cauldron, we first determine # archs inside
the cauldron, pick the correct formulalist, randomly match against that list
to see if we got a match. If matched, we remove the ingredients, and insert
the artifact into the cauldron. Exp is awarded to the players alchemy skill
(if they have it).

