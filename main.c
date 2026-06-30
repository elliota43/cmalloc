#include "arena.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t ip_address;
  uint16_t port;
  uint8_t is_active;
} Connection;

typedef struct {
  uint8_t *raw_data;
  size_t length;
} NetworkFrame;

#define ARENA_CAPACITY (1024 * 1024 * 16) // 16 MB

int main(void) {

  void *backing_mem = malloc(ARENA_CAPACITY);
  if (backing_mem == NULL) {
    fprintf(stderr, "fatal: could not allocate backing memory for arena.\n");
    return 1;
  }

  Arena arena = {0};
  arena_init(&arena, backing_mem, ARENA_CAPACITY);
  printf("Arena capacity: %d bytes.\n\n", ARENA_CAPACITY);

  Connection *conn = (Connection *)arena_alloc(&arena, sizeof(Connection));
  conn->ip_address = 0xC0A80001;
  conn->port = 8080;
  conn->is_active = 1;

  printf("Allocated connection struct at %p\n", (void *)conn);
  printf("Arena current offset: %zu bytes\n\n", arena.curr_offset);

  printf("=== Incoming network packet ===\n");

  Temp_Arena_Memory scratch = temp_arena_memory_begin(&arena);

  NetworkFrame *frame =
      (NetworkFrame *)arena_alloc_align(&arena, sizeof(NetworkFrame), 8);
  frame->length = 512;
  frame->raw_data = (uint8_t *)arena_alloc_align(&arena, frame->length, 16);

  memset(frame->raw_data, 0xFF, frame->length);
  printf("Temp: Allocated %zu byte packet frame at %p\n", frame->length,
         (void *)frame->raw_data);
  printf("Arena current offset: %zu bytes\n", arena.curr_offset);

  temp_arena_memory_end(scratch);
  printf("=== End Temp Arena ===\n\n");

  char *log_msg = (char *)arena_alloc(&arena, 64);
  strcpy(log_msg, "Connection established successfully!");
  printf("Allocated log message at %p\n", (void *)log_msg);
  printf("Message: %s\n", log_msg);

  printf("Arena final offset: %zu bytes\n\n", arena.curr_offset);

  arena_free_all(&arena);
  free(backing_mem);

  return 0;
}
