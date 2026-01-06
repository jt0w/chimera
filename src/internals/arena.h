#ifndef CHIMERA_ARENA_H
#define CHIMERA_ARENA_H
#include <stdlib.h>

typedef struct {
  size_t pos;
  size_t cap;
  void *buf;
} Chimera_Arena;

Chimera_Arena chimera_arena_create(size_t cap);
void chimera_arena_destroy(Chimera_Arena *arena);
void *chimera_arena_alloc(Chimera_Arena *arena, size_t size);

#ifdef CHIMERA_IMPLEMENTATION
Chimera_Arena chimera_arena_create(size_t cap) {
  Chimera_Arena arena = {
    .pos = 0,
    .cap = cap,
    .buf = malloc(cap),
  };
  assert(arena.buf != NULL && "Buy some RAM");
  return arena;
}

void chimera_arena_destroy(Chimera_Arena *arena) {
  free(arena->buf);
  arena->pos = 0;
  arena->cap = 0;
}


void *chimera_arena_alloc(Chimera_Arena *arena, size_t size) {
  if (arena->pos + size > arena->cap) return NULL;
  size_t pre_pos = arena->pos;
  void *out = arena->buf + arena->pos;
  arena->pos += size;
  return out;
}
#endif // CHIMERA_IMPLEMENTATION
#endif // endif CHIMERA_ARENA_H
