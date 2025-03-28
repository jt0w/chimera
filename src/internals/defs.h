#ifndef _CHIMERA_DEFS_H_
#define _CHIMERA_DEFS_H_
#include <stdlib.h>
#include <assert.h>

#ifndef CHIMERA_COMPILER
  #define CHIMERA_COMPILER "gcc"
#endif

#ifndef CHIMERA_MIN_LOG_LEVEL
  #define CHIMERA_MIN_LOG_LEVEL CHIMERA_INFO
#endif

#ifndef CHIMERA_FREE
  #define CHIMERA_FREE free
#endif

#ifndef CHIMERA_MALLOC
  #define CHIMERA_MALLOC malloc
#endif

#ifndef CHIMERA_REALLOC
  #define CHIMERA_REALLOC realloc
#endif

#ifndef CHIMERA_ASSERT
  #define CHIMERA_ASSERT assert
#endif
#endif // _CHIMERA_DEFS_H_
