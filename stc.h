// # Dynamic Arrays:
// Dynamic Array are expected to be a struct with the following fields:
// typedef struct {
//  void* items; // void* is only used to symbolise any type
//  size_t size;
//  size_t cap;
// } Some_Da;
// Example implemenations can be found when in Stc_StringBuilder (just search
// here in this file).
#ifndef _STC_H_
#define _STC_H_
#include <assert.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef STC_STRIP_PREFIX
#define todo stc_todo
#define log stc_log
#define shift stc_shift
#define da_len stc_da_len
#define da_get stc_da_get
#define da_push stc_da_push
#define da_push_buf stc_da_push_buf
#define da_push_mult stc_da_push_mult
#define da_free stc_da_free
#define LogLevel Stc_LogLevel
#define StringBuilder Stc_StringBuilder
#define Cmd Stc_Cmd
#define cmd_push stc_cmd_push
#define cmd_exec stc_cmd_exec
#define rebuild_file stc_rebuild_file
#define read_file stc_read_file
#define create_dir stc_create_dir
#define temp_alloc stc_temp_alloc
#define temp_sprintf stc_temp_sprintf
#define temp_reset stc_temp_reset
#endif

#ifndef STC_INIT_DA_CAP
#define STC_INIT_DA_CAP 256
#endif

#ifndef STC_COMPILER
#define STC_COMPILER "gcc"
#endif

#ifndef STC_MIN_LOG_LEVEL
#define STC_MIN_LOG_LEVEL STC_INFO
#endif

#ifndef STC_FREE
#define STC_FREE free
#endif

#ifndef STC_MALLOC
#define STC_MALLOC malloc
#endif

#ifndef STC_REALLOC
#define STC_REALLOC realloc
#endif

#ifndef STC_ASSERT
#define STC_ASSERT assert
#endif

#define STC_SUCCESS 1
#define STC_FAILURE 1

#ifndef STC_TEMP_ALLOC_CAP
#define STC_TEMP_ALLOC_CAP (1024 * 1024 * 9)
#endif

static char stc_temp[STC_TEMP_ALLOC_CAP];
static int stc_temp_sz = 0;

void *stc_temp_alloc(size_t n);
char *stc_temp_sprintf(const char *fmt, ...);
void stc_temp_reset();

#define stc_todo(reason) (fprintf(stderr, "TODO: %s\n", reason), exit(1))

#define stc_shift(xs, xs_sz) (assert(xs_sz > 0), xs_sz--, *xs++)

#define stc_rebuild_file(argv, argc) stc__rebuild_file(argv, argc, __FILE__)

#define stc_da_len(da) (sizeof(da) / sizeof(da[0]))
#define stc_da_get(da, i) (assert(i < stc_da_len(da)), da[i])
#define stc_da_push(da, item)                                                  \
  do {                                                                         \
    if ((da)->count >= (da)->cap) {                                            \
      (da)->cap = (da)->cap == 0 ? STC_INIT_DA_CAP : (da)->cap * 2;            \
      (da)->items =                                                            \
          STC_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));          \
      assert((da)->items != NULL && "You broke or what?");                     \
    }                                                                          \
                                                                               \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define stc_da_push_sized_buf(da, buf, buf_sz)                                 \
  do {                                                                         \
    for (int i = 0; i < buf_sz; ++i) {                                         \
      stc_da_push(da, buf[i]);                                                 \
    }                                                                          \
  } while (0)

#define stc_da_push_buf(da, buf) stc_da_push_sized_buf(da, buf, strlen(buf))

#define stc_da_push_mult(da, bufs, bufs_c)                                     \
  do {                                                                         \
    if ((da)->count + bufs_c > (da)->cap) {                                    \
      if ((da)->cap == 0) {                                                    \
        (da)->cap = STC_INIT_DA_CAP;                                           \
      }                                                                        \
      while ((da)->count + bufs_c > (da)->cap) {                               \
        (da)->cap *= 2;                                                        \
      }                                                                        \
      (da)->items =                                                            \
          STC_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));          \
      assert((da)->items != NULL);                                             \
    }                                                                          \
    memcpy((da)->items + (da)->count, (bufs),                                  \
           (bufs_c) * sizeof(*(da)->items));                                   \
    (da)->count += (bufs_c);                                                   \
  } while (0)

#define stc_da_free(da) STC_FREE(da.items)

typedef struct {
  char *items;
  size_t count;
  size_t cap;
} Stc_StringBuilder;

static inline Stc_StringBuilder sb_from_string(char *str) {
  return (Stc_StringBuilder){
      .items = str,
      .count = strlen(str),
  };
}

typedef struct {
  char **items;
  size_t count;
  size_t cap;
} Stc_Cmd;

#define stc_cmd_push(cmd, ...)                                                 \
  stc_da_push_mult(                                                            \
      cmd, ((const char *[]){__VA_ARGS__}),                                    \
      (sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char *)));

typedef enum {
  STC_DEBUG,
  STC_INFO,
  STC_WARN,
  STC_ERROR,
  STC_TRACE
} Stc_LogLevel;

void stc_log(Stc_LogLevel log_level, const char *fmt, ...);
void println(const char *fmt, ...);
void fprintln(FILE *out, const char *fmt, ...);
void stc_read_file(char *filename, Stc_StringBuilder *sb);

int stc_create_dir(char *dir);

#ifdef STC_IMPLEMENTATION
void stc_log(Stc_LogLevel log_level, const char *fmt, ...) {
  if (log_level < STC_MIN_LOG_LEVEL)
    return;

  FILE *out = stdout;
  switch (log_level) {
  case STC_DEBUG:
    fprintf(out, "[DEBUG]: ");
    break;
  case STC_INFO:
    fprintf(out, "[INFO]: ");
    break;
  case STC_WARN:
    fprintf(out, "[WARN]: ");
    break;
  case STC_ERROR:
    out = stderr;
    fprintf(out, "[ERROR]: ");
    break;
  case STC_TRACE:
    out = stderr;
    fprintf(out, "[TRACE]: ");
    break;
  }

  va_list args;
  va_start(args, fmt);
  vfprintf(out, fmt, args);
  va_end(args);

  fprintf(out, "\n");
}

void fprintln(FILE *out, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(out, fmt, args);
  va_end(args);

  fprintf(out, "\n");
}

void println(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);

  fprintf(stdout, "\n");
}

int stc_cmd_exec(Stc_Cmd *cmd) {
  Stc_StringBuilder sb = {0};
  while (cmd->count > 0) {
    char *string = stc_shift(cmd->items, cmd->count);
    stc_da_push_buf(&sb, string);
    stc_da_push(&sb, ' ');
  }
  stc_log(STC_INFO, "+ %s", sb.items);
  return system(sb.items);
}

void stc__rebuild_file(char **argv, int argc, const char *filename) {
  const char *bin_path = stc_shift(argv, argc);

  struct stat buffer;

  stat(bin_path, &buffer);
  int long bin_mtime = buffer.st_mtime;

  stat(filename, &buffer);
  int long file_mtime = buffer.st_mtime;
  if (!(file_mtime > bin_mtime))
    return;

  Stc_Cmd cmd = {0};
  stc_cmd_push(&cmd, STC_COMPILER, filename, "-o", bin_path);
  if (stc_cmd_exec(&cmd) != 0) {
    stc_log(STC_ERROR, " Failed while building %s", filename);
    exit(0);
  }
  cmd.count = 0;
  stc_cmd_push(&cmd, bin_path);
  while (argc > 0) {
    stc_cmd_push(&cmd, stc_shift(argv, argc));
  }
  exit(stc_cmd_exec(&cmd) / 256);
}

void stc_read_file(char *filename, Stc_StringBuilder *sb) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    stc_log(STC_ERROR, "´%s´ could not be opened", filename);
    exit(1);
  }
  char c;
  while ((c = fgetc(f)) != EOF) {
    stc_da_push(sb, c);
  }
  fclose(f);
}

int stc_create_dir(char *dir) {
  struct stat st = {0};

  if (stat(dir, &st) == -1) {
    return mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  return 0;
}

void *stc_temp_alloc(size_t n) {
  if (stc_temp_sz + n > STC_TEMP_ALLOC_CAP)
    return NULL;
  void *r = &stc_temp[stc_temp_sz];
  stc_temp_sz += n;
  return r;
}

char *stc_temp_sprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  STC_ASSERT(n > 0);
  char *res = stc_temp_alloc(n + 1);

  va_start(args, fmt);
  vsnprintf(res, n + 1, fmt, args);
  va_end(args);

  return res;
}

void stc_temp_reset() { stc_temp_sz = 0; }
#endif // END STC_IMPLEMTATION
#endif // END _STC_H_
