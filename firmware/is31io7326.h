#pragma once

#define TWI_BASE_ADDRESS     0x58

typedef union {
    struct {
        uint8_t dataNumber:1,
                keyState:1,
                od:3,
                pp:3;
    };
    uint8_t val;
} key_t;
