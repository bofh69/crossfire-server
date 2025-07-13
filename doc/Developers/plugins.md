# Plugins {#plugins}
Crossfire can be extended through dynamically-loadable shared library **plugins**. When starting,
Crossfire tries to load all plugins in the server plugin directory, usually
*LIBDIR/crossfire/plugins/*.

Additional information can be found at: https://wiki.cross-fire.org/server_plugin

See also:

- @subpage plugin_python "Python plugin (CFPython)"

## Plugin-Related DM Commands

- pluglist
- plugin
- plugout

For more information, see the in-game DM help.

## Making Plugins in C++
You should always include two header files in your plugin projects:

- plugin.h, which contains the declaration of most plugin-related constants and
  the required Crossfire includes;
- plugin_common.h (from the plugins/plugin_common directory), which includes the
  necessary support for Crossfire function wrappers.

All your projects should also include plugin_common.c in their build processes;
that source file contains a lot of Crossfire function wrappers you can call.
Do *not* call the callbacks sent by the Crossfire server directly - use what's
provided by plugin_common.c.

Important: a plugin should *never* directly call malloc, free, or any function
manipulating memory if the memory needs to be given to server. This breaks Windows
compatibility. Hooks are provided in case of need.

The technical documentation is in Developers/plugins.ps, and Developers/cfpython.ps
for Python plugin.

## Plugin Internals
You can write a plugin for Crossfire in any language that supports a C FFI and variadic arguments.

Crossfire plugins are dynamically-loadable shared libraries. Plugins must export all of the @ref PLUGIN_SYM.
