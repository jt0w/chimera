// Very limited cli flags parser
#ifndef FLAGS_H
#define FLAGS_H
#include "da.h"
#include "utils.h"
#include "temp.h"
#include <inttypes.h>
#include <errno.h>

typedef enum {
  CHIMERA_FLAG_ERROR,

  CHIMERA_FLAG_STRING,
  CHIMERA_FLAG_INT,
  CHIMERA_FLAG_BOOLEAN,

  CHIMERA_FLAG_COUNT,
} Chimera_FlagType;

typedef union {
  const char *str;
  int num_int;
  bool boolean;
  const char *error;
} Chimera_FlagValue;

typedef struct {
  Chimera_FlagType type;
  Chimera_FlagValue as;
  // pos in argv
  int pos;
} Chimera_Flag;

Chimera_Flag chimera_parse_flag(char **argv, int argc, const char *long_name,
                                const char *short_name, Chimera_FlagType type,
                                Chimera_FlagValue default_value);

// All the following macros assume that argv and argc are available under `argv` and `argc`
#define chimera_parse_boolean_flag(long_name, short_name, default) chimera_parse_flag((argv), (argc), (long_name),(short_name), CHIMERA_FLAG_BOOLEAN, (Chimera_FlagValue){.boolean = (default)})
#define chimera_parse_str_flag(long_name, short_name, default)     chimera_parse_flag((argv), (argc), (long_name),(short_name), CHIMERA_FLAG_STRING, (Chimera_FlagValue){.str = (default)})
#define chimera_parse_int_flag(long_name, short_name, default)     chimera_parse_flag((argv), (argc), (long_name),(short_name), CHIMERA_FLAG_INT, (Chimera_FlagValue){.num_int = (default)})


#ifdef CHIMERA_IMPLEMENTATION
#endif // CHIMERA_IMPLEMENTATION
Chimera_Flag chimera_parse_flag(char **argv, int argc, const char *long_name,
                                const char *short_name, Chimera_FlagType type,
                                Chimera_FlagValue default_value) {
  Chimera_Flag flag = {0};
  flag.type = type;
  flag.as = default_value;
  for (size_t i = 0; i < argc; ++i) {
    if (strcmp(argv[i], long_name) == 0 || strcmp(argv[i], short_name) == 0) {
      flag.pos = i;
      if (type == CHIMERA_FLAG_BOOLEAN) {
        flag.as.boolean = true;
        return flag;
      }
      if (argc == 0) {
        flag.type = CHIMERA_FLAG_ERROR;
        flag.as.error = "Expected value";
        return flag;
      }
      char *arg = argv[++i];
      assert(CHIMERA_FLAG_COUNT == 4);
      switch (type) {
      case CHIMERA_FLAG_STRING:
        if (arg[0] == '"' && arg[strlen(arg) - 1] != '"') {
          flag.type = CHIMERA_FLAG_ERROR;
          flag.as.error = chimera_temp_sprintf("String not terminated", arg);
          return flag;
        }
        flag.as.str = arg;
        return flag;
      case CHIMERA_FLAG_INT:
        errno = 0;
        flag.as.num_int = strtol(arg, NULL, 10);
        if (errno != 0) {
          flag.type = CHIMERA_FLAG_ERROR;
          flag.as.error = chimera_temp_sprintf("failed to parse ´%s´ as int", arg);
          return flag;
        }
        return flag;
      default:
        chimera_log(CHIMERA_ERROR, "UNREACHABLE: chimera_parse_flag");
        abort();
      }
    }
  }
  return flag;
}
#endif // endif FLAGS_H
