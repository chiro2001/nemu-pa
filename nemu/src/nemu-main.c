#include <common.h>
#include <string.h>
#include <sys/stat.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

#define RUN_CONFIG_FILE "./build/" CONFIG_EXT_RUN_CONFIG_FILE

char **g_argv = NULL;

int main_parse_args_file(char *filename) {
  struct stat sb;
  FILE *fp = NULL;
  int argc = 1;
  Assert((fp = fopen(RUN_CONFIG_FILE, "r")),
         "No config file provided! File not found: %s", RUN_CONFIG_FILE);
  assert(!(stat(RUN_CONFIG_FILE, &sb)));
  assert(filename);
  size_t file_size = sb.st_size;
  int filename_length = strlen(filename);
  char *arg = malloc(sizeof(char) * (file_size + 2 + filename_length));
  assert(arg);
  strcpy(arg, filename);
  arg[filename_length] = '\0';
  char *p_arg = arg + filename_length + 1;
  fread(p_arg, file_size, 1, fp);
  arg[file_size + 1 + filename_length] = '\0';
  char **argv = malloc(sizeof(char *) * 128);
  argv[0] = arg;
  g_argv = argv;
  char *p = p_arg;
  if (*p) argv[argc++] = p;
  while (*p) {
    if (*p == '\n') {
      *p = '\0';
      break;
    }
    if (*p == ' ' && *(p + 1)) {
      *p = '\0';
      argv[argc++] = p + 1;
    }
    p++;
  }
  return argc;
}

int main(int argc, char **argv) {
  /* When no args provided, find EXT_RUN_CONFIG_FILE*/
  if (argc == 1) {
    return main(main_parse_args_file(argv[0]), g_argv);
  }
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv ? argv: g_argv);
#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
