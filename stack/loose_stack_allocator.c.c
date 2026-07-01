#include "../includes/align.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct Stack Stack;
struct Stack {
  unsigned char *buf;
  size_t buf_len;
  size_t offset;
};

typedef struct Stack_Allocation_Header Stack_Allocation_Header;
struct Stack_Allocation_Header {
  uint8_t padding;
};

void stack_init(Stack *s, void *backing_buffer, size_t backing_buffer_length) {
  s->buf = (unsigned char *)backing_buffer;
  s->buf_len = backing_buffer_length;
  s->offset = 0;
}

void *stack_alloc_align(Stack *s, size_t size, size_t alignment) {
  assert(is_power_of_two(alignment));

  uintptr_t current_ptr = (uintptr_t)s->buf + s->offset;

  // where the user data begins
  uintptr_t data_start =
      align_forward(current_ptr + sizeof(Stack_Allocation_Header), alignment);

  // the header goes right before the data
  uintptr_t header_start = data_start - sizeof(Stack_Allocation_Header);
  // the padding is how much we had to push the offset forward before placing
  // the header
  uintptr_t padding = header_start - current_ptr;

  assert(padding <= 255);

  if (data_start + size > (uintptr_t)s->buf + s->buf_len) {
    return NULL; // overflow
  }

  Stack_Allocation_Header *header = (Stack_Allocation_Header *)header_start;
  header->padding = (uint8_t)padding;

  s->offset = (data_start + size) - (uintptr_t)s->buf;

  return (void *)data_start;
}

void stack_free(Stack *s, void *ptr) {
  if (ptr != NULL) {
    uintptr_t start, end, curr_addr;
    Stack_Allocation_Header *header;
    size_t prev_offset;

    start = (uintptr_t)s->buf;
    end = start + (uintptr_t)s->buf_len;
    curr_addr = (uintptr_t)ptr;

    if (!(start <= curr_addr && curr_addr < end)) {
      assert(0 &&
             "Out of bounds memory address passed to stack allocator (free)");
      return;
    }

    if (curr_addr >= (start + (uintptr_t)s->offset)) {
      // allow double frees
      return;
    }

    header = (Stack_Allocation_Header *)(curr_addr -
                                         sizeof(Stack_Allocation_Header));
    prev_offset = (size_t)(curr_addr - (uintptr_t)header->padding - start);

    s->offset = prev_offset;
  }
}

void *stack_resize_align(Stack *s, void *ptr, size_t old_size, size_t new_size,
                         size_t alignment) {
  if (ptr == NULL) {
    return stack_alloc_align(s, new_size, alignment);
  } else if (new_size == 0) {
    stack_free(s, ptr);
    return NULL;
  } else {
    uintptr_t start, end, curr_addr;
    size_t min_size = old_size < new_size ? old_size : new_size;
    void *new_ptr;

    start = (uintptr_t)s->buf;
    end = start + (uintptr_t)s->buf_len;

    curr_addr = (uintptr_t)ptr;
    if (!(start <= curr_addr && curr_addr < end)) {
      assert(0 &&
             "Out of bounds memory address passed to stack allocator (resize)");
      return NULL;
    }

    if (curr_addr >= start + (uintptr_t)s->offset) {
      // treat as double free
      return NULL;
    }

    if (old_size == new_size) {
      return ptr;
    }

    new_ptr = stack_alloc_align(s, new_size, alignment);
    memmove(new_ptr, ptr, min_size);
    return new_ptr;
  }
}

void *stack_resize(Stack *s, void *ptr, size_t old_size, size_t new_size) {
  return stack_resize_align(s, ptr, old_size, new_size, DEFAULT_ALIGNMENT);
}

void stack_free_all(Stack *s) { s->offset = 0; }
