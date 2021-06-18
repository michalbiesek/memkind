// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2016 - 2021 Intel Corporation. */

#include <memkind/internal/memkind_log.h>
#include <memkind/internal/memkind_private.h>

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

typedef enum
{
    MESSAGE_TYPE_INFO,
    MESSAGE_TYPE_ERROR,
    MESSAGE_TYPE_FATAL,
    MESSAGE_TYPE_DEBUG = MESSAGE_TYPE_FATAL,
    MESSAGE_TYPE_MAX_VALUE,
} message_type_t;

static const char *message_prefixes[MESSAGE_TYPE_MAX_VALUE] = {
    [MESSAGE_TYPE_INFO] = "MEMKIND_INFO",
    [MESSAGE_TYPE_ERROR] = "MEMKIND_ERROR",
    [MESSAGE_TYPE_FATAL] = "MEMKIND_FATAL",
};

static bool log_enabled;

static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static unsigned log_level;
static void log_init_once(void)
{
    char *memkind_debug_env = memkind_get_env("MEMKIND_DEBUG");

    if (memkind_debug_env) {
        if (strcmp(memkind_debug_env, "1") == 0) {
            log_enabled = true;
        } else {
            fprintf(
                stderr,
                "MEMKIND_WARNING: debug option \"%s\" unknown; Try man memkind for available options.\n",
                memkind_debug_env);
        }
    }
}

static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
static void log_generic(message_type_t type, const char *format, va_list args)
{
    pthread_once(&init_once, log_init_once);
    if (log_enabled || (type == MESSAGE_TYPE_FATAL)) {
        if (pthread_mutex_lock(&log_lock) != 0)
            assert(0 && "failed to acquire mutex");
        fprintf(stderr, "%s: ", message_prefixes[type]);
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
        if (pthread_mutex_unlock(&log_lock) != 0)
            assert(0 && "failed to release mutex");
    }
}

void log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_generic(MESSAGE_TYPE_INFO, format, args);
    va_end(args);
}

void log_err(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_generic(MESSAGE_TYPE_ERROR, format, args);
    va_end(args);
}

void log_fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_generic(MESSAGE_TYPE_FATAL, format, args);
    va_end(args);
}

static const char *memtier_message_prefixes[MESSAGE_TYPE_MAX_VALUE] = {
    [MESSAGE_TYPE_ERROR] = "MEMKIND_MEM_TIERING_LOG_ERROR",
    [MESSAGE_TYPE_INFO] = "MEMKIND_MEM_TIERING_LOG_INFO",
    [MESSAGE_TYPE_DEBUG] = "MEMKIND_MEM_TIERING_LOG_DEBUG",
};

static ssize_t swrite(int fd, const void *buf, size_t count)
{
    return syscall(SYS_write, fd, buf, count);
}

static unsigned memtier_log_level;
static pthread_mutex_t memtier_log_lock = PTHREAD_MUTEX_INITIALIZER;

MEMKIND_EXPORT void memtier_log_init_once(void)
{
    memtier_log_level = MESSAGE_TYPE_ERROR;

    char *log_level_env = memkind_get_env("MEMKIND_MEM_TIERING_LOG_LEVEL");
    if (log_level_env) {
        char *end;
        errno = 0;
        memtier_log_level = strtoul(log_level_env, &end, 10);
        if ((memtier_log_level >= MESSAGE_TYPE_MAX_VALUE) || (*end != '\0') ||
            (errno != 0)) {
            memtier_log_err("Wrong value of MEMKIND_MEM_TIERING_LOG_LEVEL=%s",
                            log_level_env);
            abort();
        } else {
            memtier_log_debug("Setting log level to: %u", log_level);
        }
    }
}

static void memtier_log_generic(message_type_t type, const char *format,
                                va_list args)
{
    static char buf[4096], *b;

    b = buf + sprintf(buf, "%s: ", memtier_message_prefixes[type]);
    int blen = sizeof(buf) + (buf - b) - 1;
    int len = vsnprintf(b, blen, format, args);
    sprintf(b + len, "\n");
    b += len + 1;

    if (pthread_mutex_lock(&memtier_log_lock) != 0) {
        assert(0 && "failed to acquire log mutex");
    }

    const char overflow_msg[] = "Warning: message truncated.\n";
    if (len >= blen)
        swrite(STDERR_FILENO, overflow_msg, sizeof(overflow_msg));
    swrite(STDERR_FILENO, buf, b - buf);

    if (pthread_mutex_unlock(&memtier_log_lock) != 0) {
        assert(0 && "failed to release log mutex");
    }
}

void memtier_log_info(const char *format, ...)
{
    if (log_level >= MESSAGE_TYPE_INFO) {
        va_list args;
        va_start(args, format);
        memtier_log_generic(MESSAGE_TYPE_INFO, format, args);
        va_end(args);
    }
}

void memtier_log_err(const char *format, ...)
{
    if (log_level >= MESSAGE_TYPE_ERROR) {
        va_list args;
        va_start(args, format);
        memtier_log_generic(MESSAGE_TYPE_ERROR, format, args);
        va_end(args);
    }
}

void memtier_log_debug(const char *format, ...)
{
    if (log_level >= MESSAGE_TYPE_DEBUG) {
        va_list args;
        va_start(args, format);
        memtier_log_generic(MESSAGE_TYPE_DEBUG, format, args);
        va_end(args);
    }
}