/* Very limited cli flags parser
 * Still a WIP
*/
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
  const char *long_name;
  const char *short_name;
  const char *desc;
  // pos in argv
  int pos;
} Chimera_Flag;

CHIMERA_DA_STRUCT(Chimera_Flag, Chimera_Flags)

Chimera_Flag chimera_parse_flag(Chimera_Flags *flags, char **argv, int argc, const char *long_name,
                                const char *short_name, Chimera_FlagType type,
                                Chimera_FlagValue default_value, const char *desc);

// All the following macros assume that argv and argc are available under `argv` and `argc`
#define chimera_parse_boolean_flag(flags, long_name, short_name, default, desc)  \
chimera_parse_flag(&flags, (argv), (argc), (long_name),(short_name), CHIMERA_FLAG_BOOLEAN, (Chimera_FlagValue){.boolean = (default)}, desc)

#define chimera_parse_str_flag(flags, long_name, short_name, default, desc) \
  chimera_parse_flag(&flags, (argv), (argc), (long_name),(short_name), CHIMERA_FLAG_STRING,  (Chimera_FlagValue){.str = (default)}, desc)

#define chimera_parse_int_flag(flags, long_name, short_name, default, desc)  \
chimera_parse_flag(&flags, (argv), (argc), (long_name),(short_name), CHIMERA_FLAG_INT,     (Chimera_FlagValue){.num_int = (default)}, desc)

void chimera_print_flags_help(Chimera_Flags flags);

#ifdef CHIMERA_IMPLEMENTATION
Chimera_Flag chimera_parse_flag(Chimera_Flags *flags, char **argv, int argc, const char *long_name,
                                const char *short_name, Chimera_FlagType type,
                                Chimera_FlagValue default_value, const char *desc) {
  Chimera_Flag flag = {0};
  flag.type = type;
  flag.as = default_value;
  flag.long_name = long_name;
  flag.short_name = short_name;
  flag.desc = desc;
  chimera_da_push(flags, flag);
  for (size_t i = 0; i < argc; ++i) {
    if (strcmp(argv[i], long_name) == 0 || strcmp(argv[i], short_name) == 0) {
      flag.pos = i;
      if (type == CHIMERA_FLAG_BOOLEAN) {
        flag.as.boolean = !default_value.boolean;
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

void chimera_print_flags_help(Chimera_Flags flags) {
  size_t max_short_len = 0;
  size_t max_long_len = 0;
  size_t l = 0;
  chimera_da_foreach(Chimera_Flag, flag, flags) {
    l = strlen(flag->short_name);
    if (l > max_short_len) max_short_len = l;
    l = strlen(flag->long_name);
    if (l > max_long_len) max_long_len = l;
  }
  println("Available flags:");
  chimera_da_foreach(Chimera_Flag, flag, flags) {
    println("   %-*s | %-*s -> %s", max_short_len, flag->short_name, max_long_len, flag->long_name, flag->desc);
  }
}
#endif // CHIMERA_IMPLEMENTATION
#endif // endif FLAGS_H
