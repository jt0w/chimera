#ifndef _CHIMERA_DA_H_
#define _CHIMERA_DA_H_
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#ifndef CHIMERA_INIT_DA_CAP
#define CHIMERA_INIT_DA_CAP 256
#endif

#define chimera_da_len(da) ((da).count)

#define chimera_da_get(da, i) (assert(i < chimera_da_len(da)), da[i])

#define chimera_da_push(da, item)                                              \
  do {                                                                         \
    if ((da)->count >= (da)->cap) {                                            \
      (da)->cap = (da)->cap == 0 ? CHIMERA_INIT_DA_CAP : (da)->cap * 2;        \
      (da)->items =                                                            \
          CHIMERA_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));      \
      assert((da)->items != NULL && "You broke or what?");                     \
    }                                                                          \
                                                                               \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define chimera_da_push_sized_buf(da, buf, buf_sz)                             \
  do {                                                                         \
    for (int i = 0; i < buf_sz; ++i) {                                         \
      chimera_da_push(da, buf[i]);                                             \
    }                                                                          \
  } while (0)

#define chimera_da_push_buf(da, buf)                                           \
  chimera_da_push_sized_buf(da, buf, strlen(buf))

#define chimera_da_push_mult(da, bufs, bufs_c)                                 \
  do {                                                                         \
    if ((da)->count + bufs_c > (da)->cap) {                                    \
      if ((da)->cap == 0) {                                                    \
        (da)->cap = CHIMERA_INIT_DA_CAP;                                       \
      }                                                                        \
      while ((da)->count + bufs_c > (da)->cap) {                               \
        (da)->cap *= 2;                                                        \
      }                                                                        \
      (da)->items =                                                            \
          CHIMERA_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));      \
      assert((da)->items != NULL);                                             \
    }                                                                          \
    memcpy((da)->items + (da)->count, (bufs),                                  \
           (bufs_c) * sizeof(*(da)->items));                                   \
    (da)->count += (bufs_c);                                                   \
  } while (0)

typedef struct {
  char *items;
  size_t count;
  size_t cap;
} Chimera_StringBuilder;

Chimera_StringBuilder chimera_sb_from_string(char *str);

#define chimera_da_free(da) CHIMERA_FREE(da.items)

typedef struct {
  const char *data;
  size_t count;
} Chimera_StringView;

Chimera_StringView chimera_sv_from_string(const char *str);
Chimera_StringView chimera_sv_from_data(const char *str, size_t count);

Chimera_StringView chimera_sv_trim_left(Chimera_StringView sv);
Chimera_StringView chimera_sv_trim_left(Chimera_StringView sv);
Chimera_StringView chimera_sv_trim(Chimera_StringView sv);

int chimera_sv_cmp(Chimera_StringView a, Chimera_StringView b);

#define chimera_sv_from_sb(sb) chimera_sv_from_data((sb).items, (sb).count)

// Dynamic Array of StringViews
typedef struct{
  Chimera_StringView *items;
  size_t count;
  size_t cap;
} Chimera_StringViews;

Chimera_StringViews chimera_sv_split_by_char(Chimera_StringView sv, char sep);

#ifdef CHIMERA_IMPLEMENTATION

Chimera_StringBuilder chimera_sb_from_string(char *str) {
  return (Chimera_StringBuilder){
    .items = str,
      .count = strlen(str),
  };
}
Chimera_StringView chimera_sv_from_string(const char *str) {
  return (Chimera_StringView){
      .data = str,
      .count = strlen(str),
  };
}

Chimera_StringView chimera_sv_from_data(const char *str, size_t count) {
  return (Chimera_StringView){
      .data = str,
      .count = count,
  };
}

Chimera_StringView chimera_sv_trim_left(Chimera_StringView sv) {
  size_t i = 0;
  while (isspace(sv.data[i]))
    i++;
  return chimera_sv_from_data(sv.data + i, sv.count - i);
}

Chimera_StringView chimera_sv_trim_right(Chimera_StringView sv) {
  size_t i = 0;
  while (isspace(sv.data[sv.count - i]))
    i++;
  return chimera_sv_from_data(sv.data, sv.count - i);
}

Chimera_StringView chimera_sv_trim(Chimera_StringView sv) {
  return chimera_sv_trim_left(chimera_sv_trim_right(sv));
}

int chimera_sv_cmp(Chimera_StringView a, Chimera_StringView b) {
  return memcmp(a.data, b.data, a.count);
}

Chimera_StringViews chimera_sv_split_by_char(Chimera_StringView sv, char sep) {
  Chimera_StringViews svs = {0};
  size_t last = 0;
  size_t i = 0;
  for (; i <= sv.count; ++i) {
    if (sv.data[i] == sep) {
      chimera_da_push(&svs, chimera_sv_from_data(sv.data + last + (last == 0 ? 0 : 1), i - last - (last == 0 ? 0 : 1)));
      last = i;
      continue;
    }
  }
  chimera_da_push(&svs, chimera_sv_from_data(sv.data + last + (last == 0 ? 0 : 1), i - last - (last == 0 ? 0 : 1)));
  return svs;
}

#endif // CHIMERA_IMPLEMENTATION
#endif // _CHIMERA_DA_H_
