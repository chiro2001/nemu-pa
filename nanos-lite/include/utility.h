// Written by:
// Date:

#ifndef UTILITY_H
#define UTILITY_H

// add your own #includes, #defines, typedefs, structs and
// function prototypes here

// Written by:
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef FD_NUM
#define FD_NUM
enum FD_NUM_T { FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_UNUSED };
#endif

typedef enum {
  REGULAR_FILE,
  DIRECTORY,
} FileType;

typedef enum {
  FS_OK = 0,
  FS_ERROR,
  FS_FILE_EXISTS,
  FS_NO_SUCH_FILE,
  FS_IS_A_DIRECTORY,
  FS_NOT_A_DIRECTORY,
  FS_DIRECTORY_NOT_EMPTY
} FsErrors;

struct FIL_t {
  // 文件类型：文件夹 / 文件
  FileType type;
  // 文件名
  char *name;
  // 文件名长度
  size_t name_length;
  // 当 link != NULL 的时候表示这个文件是某文件的链接
  struct FIL_t *link;
  // 上层文件
  struct FIL_t *parent;
  // 子文件列表
  struct FIL_t **children;
  // 子文件数量大小
  size_t size_children;
  // 文件大小
  size_t size_file;
  // 文件内容
  char *content;
  // 文件号
  size_t file;
  // 读取文件偏移位置
  size_t offset;
};

typedef struct FIL_t FIL;

struct PATH_t {
  struct FIL_t *file;
  struct PATH_t *forward;
  struct PATH_t *next;
};

typedef struct PATH_t PATH;

// 储存文件系统相关信息
struct FsRep {
  // 根文件目录
  FIL *root;
  // 当前目录（双向链表尾部）
  PATH *current;
  // 当前路径（双向链表头部）
  PATH *pathRoot;
};

#ifndef Fs
typedef struct FsRep *Fs;
#endif

// 分隔符
#define FS_SPLIT '/'
#define FS_SPLIT_STR "/"

#define BLUE "\033[34m"
#define RESET_COLOR "\033[0m"

/// 错误码 -> 错误描述
extern const char FsErrorMessages[8][64];

// #define DEBUG

#ifdef DEBUG
#define PERROR(code, prefix) \
  printf("[Line:%-4d] " prefix ": %s\n", __LINE__, FsErrorMessages[code]);

#define PERRORD(code, prefix, ...)                              \
  printf("[Line:%-4d] " prefix ": %s\n", __LINE__, __VA_ARGS__, \
         FsErrorMessages[code]);
#else
#define PERROR(code, prefix) printf(prefix ": %s\n", FsErrorMessages[code]);

#define PERRORD(code, prefix, ...) \
  printf(prefix ": %s\n", __VA_ARGS__, FsErrorMessages[code]);
#endif

// 文件夹最大文件数量大小
#define FS_MAX_CHILDREN 64
// 是否在列出文件时在文件夹末尾加上分隔符
// #define FS_SHOW_DIR_SPLIT

FIL *FsFilFindByName(FIL *dir, const char *name);

void FsFilSort(FIL *dir, int reverse);

void FsFilPrint(FIL *file);

PATH *FsPathGetTail(PATH *path);

void FsFilFree(FIL *file);

void FsPathFree(PATH *path);

PATH *FsPathInsert(PATH *tail, FIL *file);

void FsFilInit(FIL *parent, FIL **file, const char *name);

void FsMkLink(FIL *parent, FIL *link_to, const char *name);

void FsInitDir(FIL *parent, FIL **file, const char *name);

void FsInitFile(FIL *parent, FIL **file, const char *name);

PATH *FsPathClone(PATH *src);

char *FsPathGetStr(PATH *path);

void FsPathSimplify(PATH **path);

FsErrors FsPathParse(PATH *pathRoot, const char *pathStr, PATH **path);

char *FsPathStrGetName(char *pathStr);

char *FsPathStrShift(char *pathStr);

FsErrors FsTreeInner(FIL *file, int layer, char *pathStrInput);

void FsFilDlTree(FIL *file);

FsErrors FsFilCopy(FIL *src, FIL *dst);

FsErrors FsFilMove(FIL *src, FIL *dst);

void FsPrint(Fs fs, char *pathStr);

enum { FS_WRITE_NEW, FS_WRITE_APPEND };

int FsWrite(FIL *target, size_t size, void *src, int mode);

int FsRead(FIL *target, size_t offset, size_t size, void *dst);

extern size_t file_tail;
#define MYFS_FILE_START (FD_UNUSED + 10000)
#define MYFS_FILE_MAX 512
extern FIL *files_list[MYFS_FILE_MAX];

#endif
