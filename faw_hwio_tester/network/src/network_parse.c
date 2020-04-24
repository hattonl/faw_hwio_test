
#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

#include "debug.h"
#include "network_parse.h"
#include "network_test.h"
#include "smallargs.h"

#define UNUSED(x) ((void)x)
#define FILENAME_LEN 256

static sarg_root root;
static char filename[FILENAME_LEN];
static int got_file = 0;

static struct {
    uint8_t udp;
    uint8_t interval;
    uint16_t time;
    uint16_t port;
    uint16_t pthread_num;
    uint32_t windows_size;
    uint32_t package_size;
    char ip[MAX_IP_SIZE];
    uint8_t test_mode;
    uint32_t transmit_times;

} parse_args = {.udp = DEFAULT_UDP,
                .interval = DEFAULT_INTERVAL,
                .time = DEFAULT_TIME,
                .port = DEFAULT_PORT,
                .pthread_num = DEFAULT_THREAD,
                .windows_size = DEFAULT_WIN_SIZE,
                .package_size = DEFAULT_PACK_SIZE,
                .test_mode = DEFAULT_MODE,
                .transmit_times = DEFAULT_TRANSMIT_TIMES};

static int help_cb(const sarg_result *res) {
    UNUSED(res);
    sarg_help_print(&root);
    exit(0);
}

static int client_cb(const sarg_result *res) {
    strcpy(parse_args.ip, res->str_val);
    dprintf("Client want connect server: %s\n", parse_args.ip);
    return SARG_ERR_SUCCESS;
}

static int port_cb(const sarg_result *res) {
    parse_args.port = (uint16_t)res->uint_val;
    dprintf("Client want connect server on %u port\n", res->uint_val);
    return SARG_ERR_SUCCESS;
}

static int parallel_cb(const sarg_result *res) {
    parse_args.pthread_num = (uint16_t)res->uint_val;
    dprintf("Client want connect server on %u thread(s) \n", res->uint_val);
    return SARG_ERR_SUCCESS;
}

static int windows_cb(const sarg_result *res) {
    parse_args.windows_size = (uint32_t)res->uint_val;
    dprintf("windwos size %u port\n", res->uint_val);
    return SARG_ERR_SUCCESS;
}

static int package_size_cb(const sarg_result *res) {
    parse_args.package_size = (uint32_t)res->uint_val;
    dprintf("package size %u \n", res->uint_val);
    return SARG_ERR_SUCCESS;
}

static int udp_cb(const sarg_result *res) {
    UNUSED(res);
    parse_args.udp = 1;
    dprintf("Client run on UDP mode\n");
    return SARG_ERR_SUCCESS;
}

static int reverse_cb(const sarg_result *res) {
    UNUSED(res);
    dprintf("Client run in reverse mode (server sends, client receives)\n");
    return SARG_ERR_SUCCESS;
}
static int time_cb(const sarg_result *res) {
    parse_args.time = (uint16_t)res->uint_val;
    dprintf("Client will transmit for %u seconds\n", res->uint_val);
    return SARG_ERR_SUCCESS;
}
static int interval_cb(const sarg_result *res) {
    parse_args.interval = (uint8_t)res->uint_val;
    dprintf("%u seconds between periodic throughput reports\n", res->uint_val);
    return SARG_ERR_SUCCESS;
}

static int file_cb(const sarg_result *res) {
    got_file = 1;
    strncpy(filename, res->str_val, FILENAME_LEN);
    filename[FILENAME_LEN - 1] = '\0';
    return SARG_ERR_SUCCESS;
}

static int test_mode_cb(const sarg_result *res) {
    UNUSED(res);
    parse_args.test_mode = FUNCTION_MODE;
    dprintf("Client run in functional testing mode\n");
    return SARG_ERR_SUCCESS;
}

static int transmit_packet_times_cb(const sarg_result *res) {
    parse_args.transmit_times = (uint32_t)res->uint_val;
    dprintf(
        "Client will transmit package %d times, if function test mode is "
        "enable\n",
        res->uint_val);
    return SARG_ERR_SUCCESS;
}

static sarg_opt my_opts[] = {
    {"h", "help", "show help text", BOOL, help_cb},
    {"c", "client", "<host> run in client mode, connecting to <host>", STRING,
     client_cb},
    {"p", "port", "server port to listen on/connect to", UINT, port_cb},
    {"P", "parallel", "number of parallel client streams to run", UINT,
     parallel_cb},
    {"w", "window", "#[KMG] set window size / socket buffer size", UINT,
     windows_cb},
    {"s", "package_size", "[KB] set package size", UINT, package_size_cb},
    {"u", "udp", "use UDP rather than TCP", BOOL, udp_cb},
    {"R", "reverse", "run in reverse mode (server sends, client receives)",
     BOOL, reverse_cb},
    {"t", "time", "time in seconds to transmit for (default 10 secs)", UINT,
     time_cb},
    {"i", "interval", "seconds between periodic throughput reports", UINT,
     interval_cb},
    {"f", "file", "file to read arguments from", STRING, file_cb},
    {"F", "function", "functional testing mode (default Performance Testing)",
     BOOL, test_mode_cb},
    {"T", "times", "functional test mode transmit packet times", UINT,
     transmit_packet_times_cb},
    {NULL, NULL, NULL, INT, NULL},
};

void dump_arg_info(void) {
    dprintf("interval = %d\n", parse_args.interval);
    dprintf("ip = %s\n", parse_args.ip);
    dprintf("parallel = %d\n", parse_args.pthread_num);
    dprintf("port = %d\n", parse_args.port);
    dprintf("time = %d\n", parse_args.time);
    dprintf("udp = %d\n", parse_args.udp);
    dprintf("windows_size = %d\n", parse_args.windows_size);
    dprintf("package_size = %d\n", parse_args.package_size);
}

void parameter_assignment(struct network_test *test) {  // todo: network_test
    // 中直接包含该结构体（命名为“可变参数”），然后使用memcpy的方法来赋值
    test->interval = parse_args.interval;
    test->pthread_num = parse_args.pthread_num;
    test->port = parse_args.port;
    test->time = parse_args.time;
    test->udp = parse_args.udp;
    test->windows_size = parse_args.windows_size;
    test->package_size = parse_args.package_size;
    test->test_mode = parse_args.test_mode;
    test->transmit_times = parse_args.transmit_times;
}
// int param_parse(int argc, const char **argv)
int param_parse(int argc, const char **argv, struct network_test *test) {
    int ret = sarg_init(&root, my_opts, PROGRAM_NAME);
    assert(ret == SARG_ERR_SUCCESS);

    ret = sarg_parse(&root, argv, argc);
    if (ret != SARG_ERR_SUCCESS) {
        printf("Error: Parsing failed\n");
        sarg_help_print(&root);
        sarg_destroy(&root);
        return -1;
    }

    if (got_file) {
        ret = sarg_parse_file(&root, filename);
        if (ret != SARG_ERR_SUCCESS) {
            printf("Error: Parsing file failed\n");
            sarg_destroy(&root);
            return -1;
        }
    }

    dump_arg_info();
    parameter_assignment(test);
    sarg_destroy(&root);
    return 0;
}
