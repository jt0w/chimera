#ifndef _CHIMERA_BUILD_H
#define _CHIMERA_BUILD_H
#include <stddef.h>
#include "da.h"
#include "utils.h"

typedef struct {
  char **items;
  size_t count;
  size_t cap;
} Chimera_Cmd;

#define chimera_rebuild_file(argv, argc) chimera__rebuild_file(argv, argc, __FILE__)

int chimera_cmd_exec(Chimera_Cmd *cmd);
void chimera__rebuild_file(char **argv, int argc, const char *filename);

#define chimera_cmd_push(cmd, ...)                                                 \
  chimera_da_push_mult(                                                            \
      cmd, ((const char *[]){__VA_ARGS__}),                                    \
      (sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char *)));


#ifdef CHIMERA_IMPLEMENTATION
int chimera_cmd_exec(Chimera_Cmd *cmd) {
  Chimera_StringBuilder sb = {0};
  while (cmd->count > 0) {
    char *string = chimera_shift(cmd->items, cmd->count);
    chimera_da_push_buf(&sb, string);
    chimera_da_push(&sb, ' ');
  }
  chimera_log(CHIMERA_INFO, "+ %s", sb.items);
  return system(sb.items);
}

void chimera__rebuild_file(char **argv, int argc, const char *filename) {
  const char *bin_path = chimera_shift(argv, argc);

  struct stat buffer;

  stat(bin_path, &buffer);
  int long bin_mtime = buffer.st_mtime;

  stat(filename, &buffer);
  int long file_mtime = buffer.st_mtime;
  if (!(file_mtime > bin_mtime))
    return;

  Chimera_Cmd cmd = {0};
  chimera_cmd_push(&cmd, CHIMERA_COMPILER, filename, "-o", bin_path);
  if (chimera_cmd_exec(&cmd) != 0) {
    chimera_log(CHIMERA_ERROR, " Failed while building %s", filename);
    exit(0);
  }
  cmd.count = 0;
  chimera_cmd_push(&cmd, bin_path);
  while (argc > 0) {
    chimera_cmd_push(&cmd, chimera_shift(argv, argc));
  }
  exit(chimera_cmd_exec(&cmd) / 256);
}

#endif // END CHIMERA_IMPLEMTATION
#endif // _CHIMERA_BUILD_H
//
