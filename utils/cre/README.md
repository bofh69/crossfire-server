# Crossfire Resource Editor, aka CRE

This is a basic resource viewer/editor for Crossfire, written in C++/Qt 5.

The tool does various things:

- list all archetypes, artifacts, animations, faces, maps, quests, and so on; this includes where they are used (in maps, in dialogs, etc.)
- computes spell damage
- simulates combat between monsters and a basic player (dwarf with no equipment)
- and various other reports (in HTML format that may be copied to clipboard)

Most resources are only readonly, some may become writable at some point.

What you can edit currently:

- archetypes (through a text entry field)
- NPC dialogs (through a GUI)
- quests (through a GUI)

## Linux and similar platforms

### Requirements

- Qt 5 or newer is required.
- The `crossfire-server` must be compiled before this project.
- A `make install` of Crossfire server is required for CRE to run.

### To build

`qmake && make` should be enough to build everything.

(Builds in debug mode; change .qmake.cache to suit other needs better.)

Make sure to use qmake from Qt version 5, and not a previous version.
On Fedora for instance, you need to run `qmake-qt5`.

Hopefully the call to `qmake` will fail with a clear message if the version is not 5.

## Windows

Compilation should be possible, but was never actually attempted.

Cross-compilation from Linux is known to work, using MXE. Check the `make_win32/mxe` directory for more information.

In case of cross-build, the `arch` and `maps` repositories must be put under the `cre.exe` directory, in a `var` sub-directory.

## Mac

Status is unknown.

# Notes

PNG files in the 'resources' folder were taken from Gridarta's
`resource/system` directory and are supposed to be under a free license.

On some systems (Solaris), `-lsocket -lnsl` need to get added to the link list
for cre.  Easiest way is to just edit the Makefile (after running qmake)
and add them.
