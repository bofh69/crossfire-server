/**
 * @file
 * Log levels.
 */

#ifndef LOGGER_H
#define LOGGER_H

/** Log levels for the LOG() function. Keep these aligned with syslog priorities. */
enum LogLevel {
    llevError,    /**< Problems requiring server admin to fix */
    llevWarn,     /**< Warnings or major code issues */
    llevNotice,
    llevInfo,     /**< Information. */
    llevDebug,    /**< Only for debugging purposes. */
    llevMonster,  /**< Many many details. */
    NRLOGLEVELS,
};

extern const char *const loglevel_names[];

#endif /* LOGGER_H */
