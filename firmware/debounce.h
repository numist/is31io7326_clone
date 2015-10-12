#pragma once

#include <stdint.h>

typedef struct {
    uint8_t db0;    // counter bit 0
    uint8_t db1;    // counter bit 1
    uint8_t state;  // debounced state
} debounce_t;

/**
 * debounce --
 *    The debouncer is based on a stacked counter implementation, with each bit
 *    getting its own 2-bit counter. When a bit changes, a call to debounce
 *    will increment that bit's counter. When it overflows, the change is
 *    comitted to the final debounced state and the changed bit returned.
 *
 * args:
 *    sample - the current state
 *    debouncer - the state variables of the debouncer
 *
 * returns: bits that have changed in the final debounced state
 */
static inline uint8_t debounce(uint8_t sample, debounce_t *debouncer)
{
    uint8_t delta, changes;
    
    // Set delta to changes from last stable state
    delta = sample ^ debouncer->state;
    
    // Increment counters and reset any unchanged bits
    debouncer->db1 = ((debouncer->db1) ^ (debouncer->db0)) & delta;
    debouncer->db0  = ~(debouncer->db0) & delta;
    
    // update state & calculate returned change set
    changes = ~(~delta | (debouncer->db0) | (debouncer->db1));
    debouncer->state ^= changes;
    
    return changes;
}
