#pragma once

#define TWI_BASE_ADDRESS     0x58

typedef union {
    struct {
        uint8_t od:3,
                pp:3,
                keyState:1,
                dataNumber:1;
    };
    uint8_t val;
} key_t;
