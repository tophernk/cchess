#ifndef CCHESS_LOGGER_H
#define CCHESS_LOGGER_H

#define CCHESS_LOG "cchess.log"
#define CCHESS_LOG_LEVEL -1

typedef enum {
    LOG_FILE,
    LOG_STDOUT
} log_level_t;

void cchess_log(const char *msg, ...);

void cchess_log_file(const char *msg, ...);

#endif // CCHESS_LOGGER_H

