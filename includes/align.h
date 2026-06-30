#ifndef ALIGN_H
#define ALIGN_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline bool is_power_of_two(uintptr_t x) { return (x & (x - 1)) == 0; }

static inline uintptr_t align_forward(uintptr_t ptr, size_t align) {
  assert((align != 0) && is_power_of_two(align));
  return (ptr + (align - 1)) & ~(align - 1);
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#endif
