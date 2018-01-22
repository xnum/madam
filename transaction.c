#include "transaction.h"
#include <uv.h>
#include <string.h>
#include <stdlib.h>

struct sockaddr_in addr;
static uv_udp_t serv;
static msg_t *store = NULL;
int size = 10;
int ptr = 0;

void log_add(int id, char* text)
{
    if(ptr+1 >= size)
    {
        size *= 1.75;
        store = realloc(store, size * sizeof(msg_t));
    }

    store[ptr].mq_id = id;
    memcpy(store[ptr].text, text, 64);

    char buf[128] = {};
    snprintf(buf, 128, "%d %d %s\n", ptr, id, text);
    slogf(DEBUG, "Send %s\n", buf);
    uv_buf_t bufs = uv_buf_init(buf, 128);
    uv_udp_try_send(&serv, &bufs, 1, &addr);

    ptr++;
}

void log_init()
{
    store = calloc(size, sizeof(msg_t));

    uv_udp_init(uv_default_loop(), &serv);
    struct sockaddr_in bind_addr;
    int rc = uv_ip4_addr("0.0.0.0", "5566", &bind_addr);
    uv_udp_bind(&serv, &bind_addr, 0);

    uv_udp_set_broadcast(&serv, 1);



    uv_ip4_addr("255.255.255.255", 8899, &addr); 
}
