#pragma once
typedef const char *sstring;

/* shstr.cpp */
extern void init_hash_table(void);
extern sstring add_string(const char *str);
extern sstring add_refcount(sstring str);
extern int query_refcount(sstring str);
extern sstring find_string(const char *str);
extern void free_string(sstring str);
extern void ss_dump_statistics(char *buf, size_t size);
extern char *ss_dump_table(int what, char *buf, size_t size);
extern int buf_overflow(const char *buf1, const char *buf2, size_t bufsize);
