#pragma once
void active_add(object *ob);
void active_remove(object *ob);
int active_count(void);
bool active_has_next(void);
object *active_next(void);
bool object_is_active(object *ob);
