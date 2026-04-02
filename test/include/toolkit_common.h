/* toolkit_common.c */
void cctk_setlog(const char *logfile);
void cctk_setdatadir(const char *datadir);
void cctk_init_std_archetypes(void);
object *cctk_create_game_object(const char *archname);
void cctk_set_object_strings(object *op, const char *string);

/* Check's macros adds an extra NULL which makes compilers complain, so use our own macro */
#define FAIL(...) _ck_assert_failed(__FILE__, __LINE__, "Failed" , ## __VA_ARGS__)

#define FAIL_UNLESS(expr, ...) \
  (expr) ? \
    _mark_point(__FILE__, __LINE__) : \
    _ck_assert_failed(__FILE__, __LINE__, "Assertion '"#expr"' failed" , ## __VA_ARGS__)

#define FAIL_IF(expr, ...)\
  (expr) ? \
     _ck_assert_failed(__FILE__, __LINE__, "Failure '"#expr"' occurred" , ## __VA_ARGS__) \
     : _mark_point(__FILE__, __LINE__)
