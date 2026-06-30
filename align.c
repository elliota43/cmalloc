#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool is_power_of_two(uintptr_t x) { return (x & (x - 1)) == 0; }

uintptr_t align_forward(uintptr_t ptr, size_t align) {
  assert((align != 0) && (is_power_of_two(align)));

  return (ptr + (align - 1)) & ~(align - 1);
}
