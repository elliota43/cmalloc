#include "../includes/align.h"
#include <stdint.h>

struct Stack_Allocation_Header {
  size_t prev_offset;
  size_t padding;
};

struct Stack {
  unsigned char *buf;
  size_t buf_len;
  size_t prev_offset;
  size_t curr_offset;
};
