#ifndef _CHIMERA_DA_H_
#define _CHIMERA_DA_H_
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifndef CHIMERA_INIT_DA_CAP
#define CHIMERA_INIT_DA_CAP 256
#endif

#ifndef CHIMERA_DA_REALLOC
#define CHIMERA_DA_REALLOC CHIMERA_REALLOC
#endif

#ifndef CHIMERA_DA_FREE
#define CHIMERA_DA_FREE CHIMERA_FREE
#endif

#define CHIMERA_DA_STRUCT(type, name)                                          \
  typedef struct {                                                             \
    type *items;                                                               \
    size_t count;                                                              \
    size_t cap;                                                                \
  } name;

#define chimera_da_len(da) ((da).count)

#define chimera_da_get(da, i) (assert(i < chimera_da_len(da)), da[i])

#define chimera_da_push(da, item)                                              \
  do {                                                                         \
    if ((da)->count >= (da)->cap) {                                            \
      (da)->cap = (da)->cap == 0 ? CHIMERA_INIT_DA_CAP : (da)->cap * 2;        \
      (da)->items =                                                            \
          CHIMERA_DA_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));   \
      assert((da)->items != NULL && "You broke or what?");                     \
    }                                                                          \
                                                                               \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define chimera_da_push_sized_buf(da, buf, buf_sz)                             \
  do {                                                                         \
    for (size_t i = 0; i < buf_sz; ++i) {                                      \
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
          CHIMERA_DA_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));   \
      assert((da)->items != NULL);                                             \
    }                                                                          \
    memcpy((da)->items + (da)->count, (bufs),                                  \
           (bufs_c) * sizeof(*(da)->items));                                   \
    (da)->count += (bufs_c);                                                   \
  } while (0)

#define chimera_da_foreach(type, x, xs)                                        \
  for (type *x = xs.items; x < xs.items + xs.count; ++x)

CHIMERA_DA_STRUCT(char, Chimera_StringBuilder);

#define chimera_sb_len chimera_da_len
#define chimera_sb_push chimera_da_push
#define chimera_sb_push_sized_buf chimera_da_push_sized_buf
#define chimera_sb_push_buf chimera_da_push_buf
#define chimera_sb_push_mult chimera_da_push_mult
#define chimera_sb_foreach(x, xs) chimera_da_foreach(char, (x), (xs))

Chimera_StringBuilder chimera_sb_from_string(char *str);
void chimera_sb_pushf(Chimera_StringBuilder *sb, const char *fmt, ...);

#define chimera_da_free(da) CHIMERA_DA_FREE(da.items)

typedef struct {
  union {
    const char *data;
    char *items;
  };
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
CHIMERA_DA_STRUCT(Chimera_StringView, Chimera_StringViews);

Chimera_StringViews chimera_sv_split_by_char(Chimera_StringView sv, char sep);

#ifdef CHIMERA_IMPLEMENTATION

Chimera_StringBuilder chimera_sb_from_string(char *str) {
  return (Chimera_StringBuilder){
      .items = str,
      .count = strlen(str),
  };
}

void chimera_sb_pushf(Chimera_StringBuilder *sb, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  if (sb->count + n + 1 > sb->cap) {
    if (sb->cap == 0) {
      sb->cap = CHIMERA_INIT_DA_CAP;
    }
    while (sb->count + n + 1 > sb->cap) {
      sb->cap *= 2;
    }
    sb->items = CHIMERA_DA_REALLOC(sb->items, sb->cap * sizeof(*sb->items));
    assert(sb->items != NULL);
  }
  char *dest = sb->items + sb->count;
  va_start(args, fmt);
  vsnprintf(dest, n + 1, fmt, args);
  va_end(args);
  sb->count += n;
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
      chimera_da_push(&svs,
                      chimera_sv_from_data(sv.data + last + (last == 0 ? 0 : 1),
                                           i - last - (last == 0 ? 0 : 1)));
      last = i;
      continue;
    }
  }
  chimera_da_push(&svs,
                  chimera_sv_from_data(sv.data + last + (last == 0 ? 0 : 1),
                                       i - last - (last == 0 ? 0 : 1)));
  return svs;
}

#endif // CHIMERA_IMPLEMENTATION
#endif // _CHIMERA_DA_H_
