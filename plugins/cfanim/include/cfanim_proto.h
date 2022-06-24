/* cfanim.c */
extern "C"
CF_PLUGIN int initPlugin(const char *iversion, f_plug_api gethooksptr);
extern "C"
CF_PLUGIN void *getPluginProperty(int *type, ...);
extern "C"
CF_PLUGIN anim_move_result cfanim_runPluginCommand(object *op, char *params);
extern "C"
CF_PLUGIN int postInitPlugin(void);
extern "C"
CF_PLUGIN int cfanim_globalEventListener(int *type, ...);
extern "C"
CF_PLUGIN int eventListener(int *type, ...);
extern "C"
CF_PLUGIN int closePlugin(void);
