#ifndef _CHIMERA_TEMP_H_
#define _CHIMERA_TEMP_H_
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef CHIMERA_TEMP_ALLOC_CAP
  #define CHIMERA_TEMP_ALLOC_CAP (1024 * 1024 * 9)
#endif

static char chimera_temp[CHIMERA_TEMP_ALLOC_CAP];
static int chimera_temp_sz = 0;


void *chimera_temp_alloc(size_t n);
char *chimera_temp_sprintf(const char *fmt, ...);
void chimera_temp_reset();


#ifdef CHIMERA_IMPLEMENTATION
void *chimera_temp_alloc(size_t n) {
  if (chimera_temp_sz + n > CHIMERA_TEMP_ALLOC_CAP)
    return NULL;
  void *r = &chimera_temp[chimera_temp_sz];
  chimera_temp_sz += n;
  return r;
}

char *chimera_temp_sprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  CHIMERA_ASSERT(n > 0);
  char *res = chimera_temp_alloc(n + 1);

  va_start(args, fmt);
  vsnprintf(res, n + 1, fmt, args);
  va_end(args);

  return res;
}

void chimera_temp_reset() { chimera_temp_sz = 0; }
#endif // END CHIMERA_IMPLEMTATION
#endif // _CHIMERA_TEMP_H_
