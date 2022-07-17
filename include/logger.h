/**
 * @file
 * Log levels.
 */

#ifndef LOGGER_H
#define LOGGER_H

/** Log levels for the LOG() function. */
typedef enum LogLevel {
    llevError = 0,    /**< Error, serious thing. */
    llevInfo = 1,     /**< Information. */
    llevDebug = 2,    /**< Only for debugging purposes. */
    llevMonster = 3   /**< Many many details. */
} LogLevel;

extern const char *const loglevel_names[];

#endif /* LOGGER_H */
