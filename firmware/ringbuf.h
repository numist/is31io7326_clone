#pragma once

#include <stdint.h>
#include <stdbool.h>

void ringbuf_append(uint8_t value);
bool ringbuf_empty(void);
uint8_t ringbuf_pop(void);
