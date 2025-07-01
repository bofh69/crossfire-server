# Extending Crossfire {#extending}

There are several different ways to extend Crossfire. Here is a summary of the different options available:

| Type | Language | Update Without Restart | Local Events | Global Events | Extension Becomes GPL |
|---|---|---|---|---|---|
| Custom Type | C++ | No | Yes | No | Yes |
| Module | C++ | No | Yes | Yes | Yes |
| @subpage plugins | Any (C FFI) | Yes | Yes | Yes | Yes |
| @ref plugin_python | Python | Yes | Yes | Yes | No |

## Events
Crossfire extensions implement functionalities by "hooking" into events. See @ref EVENT_xxx.
