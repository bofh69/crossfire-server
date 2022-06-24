/* cfpython.c */
extern "C" int initPlugin(const char *iversion, f_plug_api gethooksptr);
extern "C" void *getPluginProperty(int *type, ...);
extern "C" void cfpython_runPluginCommand(object *op, const char *params);
extern "C" int postInitPlugin(void);
extern "C" int cfpython_globalEventListener(int *type, ...);
extern "C" int eventListener(int *type, ...);
extern "C" int closePlugin(void);
/* cfpython_archetype.c */
PyObject *Crossfire_Archetype_wrap(archetype *what);
/* cfpython_object.c */
void init_object_assoc_table(void);
PyObject *Crossfire_Object_wrap(object *what);
/* cfpython_party.c */
PyObject *Crossfire_Party_wrap(partylist *what);
/* cfpython_region.c */
PyObject *Crossfire_Region_wrap(region *what);
/* cfpython_map.c */
void init_map_assoc_table(void);
void Handle_Map_Unload_Hook(Crossfire_Map *map);
PyObject *Crossfire_Map_wrap(mapstruct *what);
