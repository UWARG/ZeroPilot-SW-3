#include "zp_assert.h"

static sAssertInfo g_assert_info;

void store_assert(const uint64_t *pc, const uint64_t *lr) {
    g_assert_info.pc = pc;
    g_assert_info.lr = lr;
}

void zp_assert(bool expr) {
    if (!expr) {
//#warning This should call the los hardfault handler, or something more appropriate
        while (1) {
        	MY_ASSERT_RECORD();
        }
    }
}
