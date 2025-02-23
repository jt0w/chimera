#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STC_STRIP_PREFIX
#define STC_IMPLEMENTATION
#include "stc.h"

int main(int argc, char **argv) {
  char *program_name = shift(argv, argc);
  if (argc < 1) {
    printf("Usage: <%s> <subcmd>\n", program_name);
    printf("To get a list of available subcommands use the subcommand `list`\n");
    fprintf(stderr, "Error: not enough args\n");
    return 1;
  }

  char *subcmd = shift(argv, argc);
  bool valid_cmd = false;

  if (strcmp(subcmd, "list") == 0) {
    printf("Here a list of all subcommands:\n");
    printf("    list    ->  Print a list of all subcmds\n");
    printf("    da      ->  Add all following args to a dynamic array and print each one of those\n");
    printf("    build   ->  Build an example application using stc\n");
    valid_cmd = true;
  }

  if (strcmp(subcmd, "da") == 0) {
    StringBuilder sb = {0};
    while (argc > 0) {
      char *arg = shift(argv, argc);
      da_push_buf(&sb, arg, strlen(arg));
    }
    printf("Got %zu bytes of args\n", sb.count);
    valid_cmd = true;
  }

  if (strcmp(subcmd, "build") == 0) {
    Cmd cmd = {0};
    cmd_append(&cmd, "gcc");
    cmd_append(&cmd, "-ggdb");
    cmd_append(&cmd, "-o");
    cmd_append(&cmd, "showcase");
    cmd_append(&cmd, __FILE__);
    int res = cmd_exec(&cmd);
    if (res != 0) {
      fprintf(stderr, "ERROR: Could not build %s\n", __FILE__);
      return res;
    }
    valid_cmd = true;
  }

  if (!valid_cmd) {
    fprintf(stderr,"ERROR: `%s` is not a valid subcmd\n", subcmd);
    return 1;
  };

  return 0;
}

