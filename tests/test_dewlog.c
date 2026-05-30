#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEWLOG_LEVEL 4
#define DEWLOG_IMPLEMENTATION
#include "dewlog.h"

#define BUFFER_SIZE 1024
#define TEMP_LOG_FILE "./temp.log"

/* Helpers */

static void read_log_file(char *buf, size_t bufsize)
{
    FILE *f = fopen(TEMP_LOG_FILE, "r");
    size_t n;
    assert(f != NULL);
    n = fread(buf, 1, bufsize - 1, f);
    buf[n] = 0;
    fclose(f);
}

static void cleanup(void)
{
    remove(TEMP_LOG_FILE);
}

/* Tests */

static void test_log_file_creation(void)
{
    char buf[BUFFER_SIZE];
    FILE *f;

    dewlog_open(TEMP_LOG_FILE);
    f = fopen(TEMP_LOG_FILE, "r");
    assert(f != NULL);
    fclose(f);
    dewlog_close();

    read_log_file(buf, sizeof(buf));
    assert(strstr(buf, "[INF]") != NULL);
    assert(strstr(buf, "Log file opened") != NULL);

    cleanup();
    printf("  test_log_file_creation: OK\n");
}

static void test_log_message_written(void)
{
    char buf[BUFFER_SIZE];

    dewlog_open(TEMP_LOG_FILE);
    LOG_ERROR("Test message: %d", 42);
    dewlog_close();

    read_log_file(buf, sizeof(buf));
    assert(strstr(buf, "Test message: 42") != NULL);
    assert(strstr(buf, "[ERR] ") != NULL);

    cleanup();
    printf("  test_log_message_written: OK\n");
}

static void test_log_level_trace_prefix(void)
{
    char buf[BUFFER_SIZE];

    dewlog_open(TEMP_LOG_FILE);
    LOG_ERROR("Err!");
    LOG_WARN("Warn!");
    LOG_INFO("Info!");
    LOG_DEBUG("Debug!");
    LOG_TRACE("Trace!");
    dewlog_close();

    read_log_file(buf, sizeof(buf));
    assert(strstr(buf, "[ERR] ") != NULL);
    assert(strstr(buf, "[WRN] ") != NULL);
    assert(strstr(buf, "[INF] ") != NULL);
    assert(strstr(buf, "[DBG] ") != NULL);
    assert(strstr(buf, "[TRC] ") != NULL);

    cleanup();
    printf("  test_log_level_prefix: OK\n");
}

/* Runner */

int main(void)
{
    printf("Running dewlog.h tests...\n");
    test_log_file_creation();
    test_log_message_written();
    test_log_level_trace_prefix();
    printf("All tests passed.\n");
    return 0;
}
