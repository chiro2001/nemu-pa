#include "am.h"
#include "fs.h"
#include ARCH_H

void init_on_yield(void (*target)(Event, Context *));
int bash_run_script(const char *filename);

extern Fs fs;

char input[FS_PATH_MAX + 1];
char cwd[FS_PATH_MAX + 1];

bool user_running = false;

void bash_return(Event e, Context *c) { bash(NULL); }

int bash_exec(char *pathStr) {
  user_running = true;
  // naive_uload(current, pathStr);
  uintptr_t entry = loader(current, pathStr);
  if (entry != (uintptr_t)(-1)) {
    Log("Jump to entry = 0x%08x", entry);
    ((void (*)())entry)();
  } else {
    Log("Loading script %s", pathStr);
    int ret = bash_run_script(pathStr);
    if (ret) {
      printf("Failed to execute file: %s\n", pathStr);
    }
  }
  user_running = false;
  return 0;
}

int bash_parser(char *in) {
  if (!*in) return 0;
  while (*in == ' ') in++;
  if (!*in) return 0;
  if (*in == '\n') return 0;
  // 注释
  if (*in == '#') return 0;
  char *arg = in;
  while (arg < in + FS_PATH_MAX && *arg && *arg != ' ') {
    arg++;
  }
  if (*arg == ' ') {
    *arg = '\0';
    arg++;
  }
  if (*arg == '\n') *arg = '\0';
  char *name = in;
  if (!*arg) arg = NULL;

  if (strcmp(name, "exit") == 0) {
    return 1;
  } else if (strcmp(name, "echo") == 0) {
    printf("%s\n", arg);
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
  } else if (*name == '.' && *(name + 1) == '/') {
    char *exec_name = name + 2;
    int ret = bash_exec(exec_name);
    if (ret) {
      printf("%s returns %d", exec_name, ret);
    }
  } else {
    printf("Unknown command: %s\n", name);
  }
  return 0;
}

int bash_run_script(const char *filename) {
  FILE *f = fopen(filename, "r");
  // FILE *f2 = stdin;
  if (!f) {
    printf("script %s not found!\n", filename);
    return 1;
  } else {
    Log("script %s open done, file=%d", filename, get_file_no(f));
  }
  while (fgets(input, FS_PATH_MAX, f) > 0) {
    if (!*input) break;
    char *p = input;
    while (*p) {
      if (*p == '\n' && !*(p + 1)) {
        *p = '\0';
        break;
      }
      p++;
    }
    Log("input = %s", input);
    bash_parser(input);
  }
  fclose(f);
  return 0;
}

int bash_startup() { return bash_run_script("/bin/boot"); }

void bash(Fs fs_) {
  init_on_yield(bash_return);
  if (fs_) fs = fs_;
  if (user_running) {
    puts("========== BACK TO BASH ==========");
    user_running = false;
  } else {
    puts("======= WHERECOME TO BASH ========");
    bash_startup();
  }
  if (!fs) {
    fs = FsNew();
  }
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
#ifndef __ARCH_NATIVE
    puts(input);
#endif
    to_exit = bash_parser(input);
  }
  if (!fs_) FsFree(fs);
  puts("======= BYE ========");
  panic("Bash exit.");
}
