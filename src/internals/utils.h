#ifndef _CHIMERA_UTILS_H_
#define _CHIMERA_UTILS_H_
#include "da.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define chimera_todo(reason) (fprintf(stderr, "TODO: %s\n", reason), exit(1))

#define chimera_shift(xs, xs_sz) (CHIMERA_ASSERT(xs_sz > 0), xs_sz--, *xs++)

#define chimera_array_len(xs) (sizeof(xs) / sizeof(*xs))

typedef enum {
  CHIMERA_DEBUG,
  CHIMERA_INFO,
  CHIMERA_WARN,
  CHIMERA_ERROR,
  CHIMERA_TRACE
} Chimera_LogLevel;

void chimera_log(Chimera_LogLevel log_level, const char *fmt, ...);
void println(const char *fmt, ...);
void fprintln(FILE *out, const char *fmt, ...);
void chimera_read_file(char *filename, Chimera_StringBuilder *sb);

int chimera_create_dir(char *dir);

bool chimera_file_exists(const char *filepath);

#ifdef CHIMERA_IMPLEMENTATION
void chimera_log(Chimera_LogLevel log_level, const char *fmt, ...) {
  if (log_level < CHIMERA_MIN_LOG_LEVEL)
    return;

  FILE *out = stdout;
  switch (log_level) {
  case CHIMERA_DEBUG:
    fprintf(out, "[DEBUG]: ");
    break;
  case CHIMERA_INFO:
    fprintf(out, "[INFO]: ");
    break;
  case CHIMERA_WARN:
    fprintf(out, "[WARN]: ");
    break;
  case CHIMERA_ERROR:
    out = stderr;
    fprintf(out, "[ERROR]: ");
    break;
  case CHIMERA_TRACE:
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

void chimera_read_file(char *filename, Chimera_StringBuilder *sb) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    chimera_log(CHIMERA_ERROR, "´%s´ could not be opened", filename);
    exit(1);
  }
  char c;
  while ((c = fgetc(f)) != EOF) {
    chimera_da_push(sb, c);
  }
  fclose(f);
}

int chimera_create_dir(char *dir) {
  struct stat st = {0};

  if (stat(dir, &st) == -1) {
    return mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  return 0;
}

bool chimera_file_exists(const char *filepath) {
  return access(filepath, F_OK) == 0;
}
#endif // END CHIMERA_IMPLEMTATION
#endif // _CHIMERA_UTILS_H_
