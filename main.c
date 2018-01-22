#include <uv.h>
#include "logger.h"

int add_match(const char* buf, int len)
{
    int idx, user_id, stk_id, is_buy, price, num;
    sscanf(buf, "%d %d %d %d %d %d", &idx,
            &user_id, &stk_id, &is_buy, &price, &num);

    slogf(INFO, "order = %*.*s\n", len, len, buf);
    char b[64] = {};
    snprintf(b, 64, "%d ADDED", idx);
    log_add(idx, b);

    return idx;
}

int main(int argc, char **argv)
{
    cli_init(argv[1], atoi(argv[2]));

    log_init();

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
