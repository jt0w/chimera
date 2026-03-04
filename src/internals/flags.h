/* Very limited cli flags parser
 * Still a WIP
 */
#ifndef FLAGS_H
#define FLAGS_H
#include "da.h"
#include "temp.h"
#include "utils.h"
#include <errno.h>
#include <inttypes.h>

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

Chimera_Flag chimera_parse_flag(Chimera_Flags *flags, char **argv, int argc,
                                const char *long_name, const char *short_name,
                                Chimera_FlagType type,
                                Chimera_FlagValue default_value,
                                const char *desc);

// All the following macros assume that argv and argc are available under `argv`
// and `argc`
#define chimera_parse_boolean_flag(flags, long_name, short_name, default,      \
                                   desc)                                       \
  chimera_parse_flag(&flags, (argv), (argc), (long_name), (short_name),        \
                     CHIMERA_FLAG_BOOLEAN,                                     \
                     (Chimera_FlagValue){.boolean = (default)}, desc)

#define chimera_parse_str_flag(flags, long_name, short_name, default, desc)    \
  chimera_parse_flag(&flags, (argv), (argc), (long_name), (short_name),        \
                     CHIMERA_FLAG_STRING,                                      \
                     (Chimera_FlagValue){.str = (default)}, desc)

#define chimera_parse_int_flag(flags, long_name, short_name, default, desc)    \
  chimera_parse_flag(&flags, (argv), (argc), (long_name), (short_name),        \
                     CHIMERA_FLAG_INT,                                         \
                     (Chimera_FlagValue){.num_int = (default)}, desc)

bool chimera_flags_check(Chimera_Flags flags, int argc);
void chimera_flags_err_str(Chimera_Flags flags, Chimera_StringBuilder *sb,
                           char **argv, int argc);

void chimera_print_flags_help(Chimera_Flags flags);

#ifdef CHIMERA_IMPLEMENTATION
Chimera_Flag chimera_parse_flag(Chimera_Flags *flags, char **argv, int argc,
                                const char *long_name, const char *short_name,
                                Chimera_FlagType type,
                                Chimera_FlagValue default_value,
                                const char *desc) {
  Chimera_Flag flag = {0};
  flag.type = type;
  flag.as = default_value;
  flag.long_name = long_name;
  flag.short_name = short_name;
  flag.pos = -1;
  flag.desc = desc;
  for (size_t i = 0; i < argc; ++i) {
    if (strcmp(argv[i], long_name) == 0 || strcmp(argv[i], short_name) == 0) {
      flag.pos = i;
      if (type == CHIMERA_FLAG_BOOLEAN) {
        flag.as.boolean = !default_value.boolean;
        goto end;
      }
      if (i + 1 >= argc) {
        flag.as.error = "Expected value";
        goto fail;
      }
      char *arg = argv[++i];
      assert(CHIMERA_FLAG_COUNT == 4);
      switch (type) {
      case CHIMERA_FLAG_STRING:
        flag.as.str = arg;
        goto end;
      case CHIMERA_FLAG_INT:
        errno = 0;
        flag.as.num_int = strtol(arg, NULL, 10);
        if (errno != 0) {
          flag.as.error =
              chimera_temp_sprintf("failed to parse ´%s´ as int", arg);
          goto fail;
        }
        goto end;
      default:
        chimera_log(CHIMERA_ERROR, "UNREACHABLE: chimera_parse_flag");
        abort();
      }
    }
  }
end:
  chimera_da_push(flags, flag);
  return flag;
fail:
  flag.type = CHIMERA_FLAG_ERROR;
  goto end;
}

void chimera_print_flags_help(Chimera_Flags flags) {
  size_t max_short_len = 0;
  size_t max_long_len = 0;
  size_t l = 0;
  chimera_da_foreach(Chimera_Flag, flag, flags) {
    l = strlen(flag->short_name);
    if (l > max_short_len)
      max_short_len = l;
    l = strlen(flag->long_name);
    if (l > max_long_len)
      max_long_len = l;
  }
  println("Available flags:");
  chimera_da_foreach(Chimera_Flag, flag, flags) {
    println("   %-*s | %-*s -> %s", max_short_len, flag->short_name,
            max_long_len, flag->long_name, flag->desc);
  }
}

bool chimera_flags_check(Chimera_Flags flags, int argc) {
  int count = 0;
  chimera_da_foreach(Chimera_Flag, flag, flags) {
    if (flag->type == CHIMERA_FLAG_ERROR)
      return false;
    if (flag->pos == -1)
      continue;
    switch (flag->type) {
    case CHIMERA_FLAG_STRING:
      count += 2;
      break;
    case CHIMERA_FLAG_INT:
      count += 2;
      break;
    case CHIMERA_FLAG_BOOLEAN:
      count += 1;
      break;
    }
  }
  return argc == count;
}

void chimera_flags_err_str(Chimera_Flags flags, Chimera_StringBuilder *sb,
                           char **argv, int argc) {
  chimera_da_foreach(Chimera_Flag, flag, flags) {
    if (flag->type == CHIMERA_FLAG_ERROR) {
      chimera_sb_push_buf(
          sb, chimera_temp_sprintf("Option `%s`/`%s`: %s", flag->long_name,
                                   flag->short_name, flag->as.error));
    }
  }

  for (size_t i = 0; i < argc;) {
    const char *arg = argv[i];
    bool found = false;
    chimera_da_foreach(Chimera_Flag, flag, flags) {
      if (strcmp(arg, flag->long_name) == 0 ||
          strcmp(arg, flag->short_name) == 0) {
        found = true;
        switch (flag->type) {
        case CHIMERA_FLAG_STRING:
          i += 2;
          break;
        case CHIMERA_FLAG_INT:
          i += 2;
          break;
        case CHIMERA_FLAG_BOOLEAN:
          i += 1;
          break;
        }
        break;
      }
    }
    if (!found) {
      chimera_sb_push_buf(sb, chimera_temp_sprintf("Unknown option `%s`", arg));
      return;
    }
  }
}
#endif // CHIMERA_IMPLEMENTATION
#endif // endif FLAGS_H
