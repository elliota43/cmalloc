#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ======== TYPES =========
// ========================

typedef struct {
  unsigned char *buf;
  size_t buf_len;
  size_t prev_offset;
  size_t curr_offset;
} Arena;

typedef struct {
  Arena *arena;
  size_t prev_offset;
  size_t curr_offset;
} Temp_Arena_Memory;

// ======= ARENA API ========
// ==========================

void arena_init(Arena *a, void *backing_buffer, size_t backing_buffer_length);

void *arena_alloc(Arena *a, size_t size);
void *arena_alloc_align(Arena *a, size_t size, size_t align);

void *arena_resize(Arena *a, void *old_memory, size_t old_size,
                   size_t new_size);
void *arena_resize_align(Arena *a, void *old_memory, size_t old_size,
                         size_t new_size, size_t align);

// arena_free is no-op
void arena_free(Arena *a, void *ptr);
void arena_free_all(Arena *a);

// ======== TEMP MEMORY API =========
// ==================================

Temp_Arena_Memory temp_arena_memory_begin(Arena *a);
void temp_arena_memory_end(Temp_Arena_Memory temp);

#ifdef __cplusplus
}
#endif
#endif // ARENA_H
