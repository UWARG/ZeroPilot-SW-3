#pragma once

#ifndef ZP_ASSERT_H
#define ZP_ASSERT_H

#include <stdint.h>

typedef struct sAssertInfo {
    uint64_t pc;
    uint64_t lr;
} sAssertInfo;

void store_assert(const uint64_t *pc, const uint64_t *lr);

#define GET_LR() __builtin_return_address(0)
//#define GET_PC(_a) __asm volatile ("mov %0, pc" : "=r" (_a)) // Comment out this for ARM
#define GET_PC(_a) __asm volatile("1: lea 1b(%%rip), %0;": "=a"(pc));  // Comment this for x86

#define MY_ASSERT_RECORD()       \
    do {                         \
      void *pc = NULL;           \
      GET_PC(pc);            	 \
      const void *lr = GET_LR(); \
      store_assert(pc, lr);         \
    } while (0)

void zp_assert(bool expr);

#endif
