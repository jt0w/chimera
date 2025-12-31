#define CHIMERA_IMPLEMENTATION
#define CHIMERA_STRIP_PREFIX
#include "src/chimera.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  rebuild_file(argv, argc);
  const char *program_name = shift(argv, argc);

  Chimera_Flag list  = parse_boolean_flag("list", "l", false);
  Chimera_Flag da    = parse_boolean_flag("dynamic_array", "da", false);
  Chimera_Flag build = parse_boolean_flag("build", "b", false);
  Chimera_Flag read  = parse_boolean_flag("read", "r", false);
  Chimera_Flag split = parse_boolean_flag("split", "s", false);

  if (list.as.boolean) {
    printf("Here a list of all subcommands:\n");
    printf("    list               ->  Print a list of all subcmds\n");
    printf(
        "    da <arg> <arg> ... ->  Add all following args to a dynamic array "
        "and print each one of those\n");
    printf("    build <file.c>     ->  Build a c file\n");
    printf("    read  <file>       ->  Read the contents of a file\n");
    printf(
        "    split <file>       ->  Read the contents of a file and split it "
        "by space\n");
    return 0;
  }

  if (da.as.boolean) {
    StringBuilder sb = {0};
    if (da.pos + 1 >= argc) {
      log(CHIMERA_ERROR,
          "Expected list of args like this: `<arg> <arg> <arg> ..`");
      return 1;
    }
    for (int i = da.pos + 1; i < argc; ++i) {
      char *buf = argv[i];
      da_push_buf(&sb, buf);
    }
    printf("Got %zu bytes of args\n", sb.count);
    da_free(sb);
    return 0;
  }

  if (build.as.boolean) {
    if (build.pos + 1 >= argc) {
      log(CHIMERA_ERROR, "Expected c file");
      return 1;
    }
    char *file = argv[build.pos + 1];
    Cmd cmd = {0};
    cmd_push(&cmd, "gcc");
    cmd_push(&cmd, file);
    if (!cmd_exec(&cmd))
      return 1;
    return 0;
  }

  if (read.as.boolean) {
    if (read.pos + 1 >= argc) {
      log(CHIMERA_ERROR, "Expected filename");
      return 1;
    }
    char *file = argv[build.pos + 1];
    StringBuilder sb = {0};
    read_file(file, &sb);
    log(CHIMERA_INFO, "Read %d bytes", sb.count);
    printf("%s\n", sb.items);
  }

  if (split.as.boolean) {
    if (split.pos + 1 >= argc) {
      log(CHIMERA_ERROR, "Expected filename");
      return 1;
    }
    char *file = argv[build.pos + 1];
    StringBuilder sb = {0};
    read_file(file, &sb);
    StringView sv = sv_from_sb(sb);
    StringViews svs = sv_split_by_char(sv, ' ');
    for (size_t i = 0; i < svs.count; ++i) {
      printf("<");
      for (size_t j = 0; j < svs.items[i].count; ++j) {
        if (svs.items[i].data[j] == '\n')
          printf("\\n");
        else
          printf("%c", svs.items[i].data[j]);
      }
      printf(">");
      printf("\n");
    }
    return 0;
  }

  log(CHIMERA_ERROR, "No valid cmd was specified");
  log(CHIMERA_ERROR, "To get a list of cmds use `%s list`", program_name);
  return 1;
}
