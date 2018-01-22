#include <uv.h>
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>

uv_tcp_t client;
uv_timer_t timer;
int last_idx = 0;

static void handle_close(uv_handle_t *cli)
{
    exit(0);
}

static void on_cli_read(uv_stream_t *cli, ssize_t st, const uv_buf_t *buf)
{
    if(st < 0) {
        slogf(ERR, "%s\n", se(st));
        uv_close(&client, handle_close);
        return;
    }

    buffer_t t = client.data;
    buffer_produced(t, st);
    while(1) {
        char *buf = buffer_begin(t);
        int found = 0;
        for(int i = 0; i < buffer_size(t) && !found; ++i) {
           if(buf[i] == '\n') {
               int c = add_match(buf, i);
               last_idx = c+1;
               buffer_consume(t, i+1);
               found = 1;
           } 
        }
        if(found) continue;
        break;
    } 
}

static void on_time(uv_timer_t* t)
{
    char buf[32] = {};
    snprintf(buf, 32, "GETA %d 0\n", last_idx);

    slogf(DEBUG, "write %s\n", buf);

    uv_buf_t b = uv_buf_init(buf, strlen(buf));

    uv_try_write(&client, &b, 1);
}

static void on_cli_conn(uv_connect_t *conn, int st)
{
    if(st < 0) {
        slogf(ERR, "%s\n", se(st));
        return;
    }

    int rc = uv_read_start((uv_stream_t*)&client, buffer_alloc, on_cli_read);

    uv_timer_init(uv_default_loop(), &timer);
    uv_timer_start(&timer, on_time, 1000, 1000);
}

int cli_init(const char *host, int port)
{
    struct sockaddr_in bind_addr;

    int rc = uv_ip4_addr(host, port, &bind_addr);

    uv_tcp_init(uv_default_loop(), &client);

    uv_connect_t *conn = malloc(sizeof(uv_connect_t));

    client.data = buffer_init();

    int e = uv_tcp_connect(conn, &client, (struct sockaddr*) &bind_addr, on_cli_conn);

    return e;
}
