#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHIMERA_IMPLEMENTATION
#define CHIMERA_STRIP_PREFIX
// Not needed since the default is CHIMERA_INFO
// #define CHIMERA_MIN_LOG_LEVEL CHIMERA_INFO
#include "src/chimera.h"

int main(int argc, char **argv) {
  rebuild_file(argv, argc);
  char *program_name = shift(argv, argc);
  if (argc < 1) {
    printf("Usage: <%s> <subcmd>\n", program_name);
    printf(
        "To get a list of available subcommands use the subcommand `list`\n");
    log(CHIMERA_ERROR, "Not enough args");
    return 1;
  }

  char *subcmd = shift(argv, argc);
  bool valid_cmd = false;

  if (strcmp(subcmd, "list") == 0) {
    printf("Here a list of all subcommands:\n");
    printf("    list             ->  Print a list of all subcmds\n");
    printf("    da               ->  Add all following args to a dynamic array "
           "and print each one of those\n");
    printf("    build <file.c>   ->  Build a c file\n");
    printf("    time <file>      ->  Get Timestamp of a file\n");
    printf("    read <file>      ->  Read the contents of a file\n");
    valid_cmd = true;
  }

  if (strcmp(subcmd, "da") == 0) {
    StringBuilder sb = {0};
    while (argc > 0) {
      char *arg = shift(argv, argc);
      da_push_buf(&sb, arg);
    }
    printf("Got %zu bytes of args\n", sb.count);
    da_free(sb);
    valid_cmd = true;
  }

  if (strcmp(subcmd, "build") == 0) {
    char *file = shift(argv, argc);
    Cmd cmd = {0};
    cmd_push(&cmd, "gcc");
    cmd_push(&cmd, file);
    if (!cmd_exec(&cmd))
      return 1;
    valid_cmd = true;
  }

  if (strcmp(subcmd, "time") == 0) {
    char *file = shift(argv, argc);
    struct stat buffer;
    int status;
    status = stat(file, &buffer);
    printf("st_mtime => %ld", buffer.st_mtime);
    valid_cmd = true;
  }

  if (strcmp(subcmd, "read") == 0) {
    char *file = shift(argv, argc);
    StringBuilder sb = {0};
    read_file(file, &sb);
    log(CHIMERA_INFO, "Read %d bytes", sb.count);
    printf("%s\n", sb.items);
    valid_cmd = true;
  }

  if (!valid_cmd) {
    fprintf(stderr, "ERROR: `%s` is not a valid subcmd\n", subcmd);
    return 1;
  };

  return 0;
}
