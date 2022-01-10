#include "fs.h"

extern Fs fs;

void bash(Fs fs_) {
  fs = fs_;
  puts("======= WHERECOME TO BASH ========");
  if (!fs) {
    fs = FsNew();
  }
  char input[FS_PATH_MAX];
  char cwd[FS_PATH_MAX + 1];
  int to_exit = 0;
  while (!to_exit) {
    FsGetCwd(fs, cwd);
    printf("\n%s # ", cwd);
    fflush(stdout);
    fgets(input, FS_PATH_MAX, stdin);
    if (*input == 0xFF) {
      Log("Invalid input!");
      size_t i = 0xFFFFFF;
      while (i--)
        ;
      continue;
    }
    char *tmp = input;
    while (*tmp) {
      if (*tmp == '\n') {
        *tmp = '\0';
        break;
      }
      tmp++;
    }
    char *arg = input;
    if (!*input) continue;
    while (arg < input + FS_PATH_MAX && *arg && *arg != ' ') {
      arg++;
    }
    if (*arg == ' ') {
      *arg = '\0';
      arg++;
    }
    char *name = input;
    if (!*arg) arg = NULL;

    if (strcmp(name, "exit") == 0) {
      to_exit = 1;
    } else if (strcmp(name, "mkdir") == 0) {
      FsMkdir(fs, arg);
    } else if (strcmp(name, "cd") == 0) {
      FsCd(fs, arg);
    } else if (strcmp(name, "pwd") == 0) {
      FsPwd(fs);
    } else if (strcmp(name, "tree") == 0) {
      FsTree(fs, arg);
    } else if (strcmp(name, "ls") == 0) {
      FsLs(fs, arg);
    } else if (strcmp(name, "cat") == 0) {
      FsCat(fs, arg);
    } else if (strcmp(name, "put") == 0) {
      char *content = arg;
      while (*content && *content != ' ') content++;
      if (*content == ' ') {
        *content = '\0';
        content++;
      } else {
        content = "";
      }
      FsPut(fs, arg, content);
    } else if (strcmp(name, "rmdir") == 0) {
      FsDldir(fs, arg);
    } else if (strcmp(name, "mkfile") == 0) {
      FsMkfile(fs, arg);
    } else if (strcmp(name, "rm") == 0) {
      bool recursive = false;
      if (*arg == '-' && *(arg + 1) == 'r' && *(arg + 2) == ' ' && *(arg + 3)) {
        recursive = true;
        arg += 3;
      }
      FsDl(fs, recursive, arg);
    } else if (strcmp(name, "cp") == 0) {
      bool recursive = false;
      if (*arg == '-' && *(arg + 1) == 'r' && *(arg + 2) == ' ' && *(arg + 3)) {
        recursive = true;
        arg += 3;
      }
      char *arg2 = arg;
      while (*arg && *arg != ' ') arg++;
      if (*arg == ' ') *(arg++) = '\0';
      char *srcFiles[] = {arg2, NULL};
      FsCp(fs, recursive, srcFiles, arg);
    } else if (strcmp(name, "mv") == 0) {
      char *arg2 = arg;
      while (*arg && *arg != ' ') arg++;
      if (*arg == ' ') *(arg++) = '\0';
      char *srcFiles[] = {arg2, NULL};
      FsMv(fs, srcFiles, arg);
    } else {
      printf("Unknown command: %s\n", name);
    }
  }
  if (!fs_) FsFree(fs);
  puts("======= BYE ========");
}
