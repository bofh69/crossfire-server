/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "global.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/**
 * One message.
 */
typedef struct i18n_message {
    sstring code;       /**< Message code, usually the English version. */
    sstring message;    /**< Message to display. */
} i18n_message;

/**
 * One available language.
 */
typedef struct i18n_file {
    sstring code;                   /**< Language code, "message." extension. */
    sstring name;                   /**< Language's name, in its native version. */
    int count;                      /**< How many items in messages. */
    struct i18n_message *messages;   /**< Available messages for this language. */
} i18n_file;

/** Defined languages. */
static std::vector<i18n_file *> i18n_files;
/** "English" language. */
static i18n_file *i18n_default = nullptr;

static int i18n_message_compare_code(const i18n_message *a, const i18n_message *b) {
    return strcmp(a->code, b->code);
}

/**
 * Translate a message in the appropriate language.
 * @param who who to translate for.
 * @param code string to translate, usually the English version.
 * @return translated message, or code if not found or who's language is invalid.
 */
const char *i18n(const object *who, const char *code) {
    i18n_message search, *found;

    if (!who || !who->contr || !who->contr->language)
        return code;

    search.code = add_string(code);

    found = static_cast<i18n_message *>(bsearch(&search, static_cast<i18n_file *>(who->contr->language)->messages, static_cast<i18n_file *>(who->contr->language)->count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code));

    free_string(search.code);

    if (found)
        return found->message;

    return code;
}

/**
 * Attempt to find the identifier of a language from its code.
 * @param code language code.
 * @return index, -1 if not found.
 */
language_t i18n_find_language_by_code(const char *code) {
    for (auto language : i18n_files) {
        if (strcmp(code, language->code) == 0)
            return language;
    }

    return nullptr;
}

/**
 * Find the identifier of a language from its code.
 * @param code language's code.
 * @return language's code, or the default language if code is invalid.
 */
language_t i18n_get_language_by_code(const char *code) {
    language_t found = i18n_find_language_by_code(code);
    if (found)
        return found;
    return i18n_default;
}

/**
 * Return the code of a specified language.
 * @param language identifier of the language.
 * @return language's code, or default language's code if identifier is invalid.
 */
sstring i18n_get_language_code(language_t language) {
    if (!language)
        return i18n_default->code;
    return static_cast<i18n_file *>(language)->code;
}

/**
 * List all languages for who.
 * @param who who to display languages for.
 */
void i18n_list_languages(object *who) {
    for (auto language : i18n_files) {
        draw_ext_info_format(NDI_UNIQUE, 0, who, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            "[fixed]%s: %s",
            language->code,
            language->name
            );
    }
}

/**
 * Replaces '\n' by a newline char.
 *
 * Since we are replacing 2 chars by 1, no overflow should happen.
 *
 * @param line
 * text to replace into.
 */
static void convert_newline(char *line) {
    char *next;
    char buf[MAX_BUF];

    while ((next = strstr(line, "\\n")) != NULL) {
        *next = '\n';
        *(next+1) = '\0';
        snprintf(buf, MAX_BUF, "%s%s", line, next+2);
        strcpy(line, buf);
    }
}

/**
 * Initializes the i18n subsystem.
 * Will load all found strings.
 * If there is an error, calls fatal().
 */
void i18n_init(void) {
    char dirname[MAX_BUF], filename[MAX_BUF*2], *line;
    BufferReader *br;
    char *token;
    DIR *dir;
    struct dirent *file;
    i18n_message code, *found;

    snprintf(dirname, sizeof(dirname), "%s/i18n/", settings.datadir);

    dir = opendir(dirname);
    if (dir == NULL) {
        LOG(llevError, "i18n: couldn't open %s\n", dirname);
        fatal(SEE_LAST_ERROR);
    }

    code.code = add_string("LN");

    while ((file = readdir(dir)) != NULL) {
        if (strncmp(file->d_name, "messages.", 9) != 0)
            continue;

        snprintf(filename, sizeof(filename), "%s%s", dirname, file->d_name);
        br = bufferreader_init_from_file(NULL, filename, "i18n: couldn't open %s: %s\n", llevError);
        if (!br) {
            fatal(SEE_LAST_ERROR);
        }
        i18n_file *language = static_cast<i18n_file *>(calloc(1, sizeof(i18n_file)));
        i18n_files.push_back(language);

        if (!language) {
            LOG(llevError, "i18n: couldn't allocate memory!\n");
            fatal(OUT_OF_MEMORY);
        }
        language->code = add_string(file->d_name + 9);
        language->count = 0;
        language->messages = NULL;

        while ((line = bufferreader_next_line(br)) != NULL) {
            if (line[0] != '#' && line[0] != '\0') {
                language->messages = static_cast<i18n_message *>(realloc(language->messages, (language->count + 1) * sizeof(i18n_message)));

                token = strtok(line, "|");
                convert_newline(token);
                language->messages[language->count].code = add_string(token);
                token = strtok(NULL, "|");
                if (token != NULL) {
                    convert_newline(token);
                    language->messages[language->count].message = add_string(token);
                } else {
                    language->messages[language->count].message = add_refcount(language->messages[language->count].code);
                }
                language->count++;
            }
        }
        bufferreader_destroy(br);

        qsort(language->messages, language->count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code);
        found = static_cast<i18n_message *>(bsearch(&code, language->messages, language->count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code));
        if (found == NULL) {
            LOG(llevError, "i18n: no language set in %s\n", filename);
            fatal(SEE_LAST_ERROR);
        }

        language->name = found->message;
        LOG(llevDebug, "i18n: %d strings for %s\n",
                language->count, found->message);

        if (strcmp(language->code, "en") == 0)
            i18n_default = language;
    }
    closedir(dir);

    free_string(code.code);

    if (i18n_default == nullptr) {
        LOG(llevError, "i18n: couldn't find default language (en)\n");
        fatal(SEE_LAST_ERROR);
    }
}

/**
 * Clears all i18n-related data.
 */
void i18n_free(void) {
  int message;

  for (auto language : i18n_files) {
      free_string(language->code); /* name is a copy of a message */
      for (message = 0; message < language->count; message++) {
          free_string(language->messages[message].code);
          free_string(language->messages[message].message);
      }
      free(language->messages);
  }
  i18n_files.clear();
}
