#ifndef _CHIMERA_DA_H_
#define _CHIMERA_DA_H_
#include <stddef.h>
#include <string.h>

#ifndef CHIMERA_INIT_DA_CAP
#define CHIMERA_INIT_DA_CAP 256
#endif

#define chimera_da_len(da) ((da).count)

#define chimera_da_get(da, i) (assert(i < chimera_da_len(da)), da[i])

#define chimera_da_push(da, item)                                                  \
  do {                                                                             \
    if ((da)->count >= (da)->cap) {                                                \
      (da)->cap = (da)->cap == 0 ? CHIMERA_INIT_DA_CAP : (da)->cap * 2;            \
      (da)->items =                                                                \
          CHIMERA_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));          \
      assert((da)->items != NULL && "You broke or what?");                         \
    }                                                                              \
                                                                                   \
    (da)->items[(da)->count++] = (item);                                           \
  } while (0)

#define chimera_da_push_sized_buf(da, buf, buf_sz)                                 \
  do {                                                                             \
    for (int i = 0; i < buf_sz; ++i) {                                             \
      chimera_da_push(da, buf[i]);                                                 \
    }                                                                              \
  } while (0)

#define chimera_da_push_buf(da, buf) chimera_da_push_sized_buf(da, buf, strlen(buf))

#define chimera_da_push_mult(da, bufs, bufs_c)                                     \
  do {                                                                             \
    if ((da)->count + bufs_c > (da)->cap) {                                        \
      if ((da)->cap == 0) {                                                        \
        (da)->cap = CHIMERA_INIT_DA_CAP;                                           \
      }                                                                            \
      while ((da)->count + bufs_c > (da)->cap) {                                   \
        (da)->cap *= 2;                                                            \
      }                                                                            \
      (da)->items =                                                                \
          CHIMERA_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));          \
      assert((da)->items != NULL);                                                 \
    }                                                                              \
    memcpy((da)->items + (da)->count, (bufs),                                      \
           (bufs_c) * sizeof(*(da)->items));                                       \
    (da)->count += (bufs_c);                                                       \
  } while (0)

typedef struct {
  char *items;
  size_t count;
  size_t cap;
} Chimera_StringBuilder;

static inline Chimera_StringBuilder sb_from_string(char *str) {
  return (Chimera_StringBuilder){
      .items = str,
      .count = strlen(str),
  };
}


#define chimera_da_free(da) CHIMERA_FREE(da.items)
#endif // _CHIMERA_DA_H_
