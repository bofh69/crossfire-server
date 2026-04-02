/* cfnewspaper.c */
extern "C" int initPlugin(const char *iversion, f_plug_api gethooksptr);
extern "C" void *getPluginProperty(int *type, ...);
extern "C" int cfnewspaper_runPluginCommand(object *op, char *params);
extern "C" int cfnewspaper_globalEventListener(int *type, ...);
extern "C" int postInitPlugin(void);
extern "C" int eventListener(int *type, ...);
extern "C" int closePlugin(void);
