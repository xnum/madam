#pragma once
#include "logger.h"

typedef struct msg_s {
    int mq_id;
    char text[64];
} msg_t;
