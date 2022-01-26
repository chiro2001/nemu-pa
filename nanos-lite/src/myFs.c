// Implementation of the File System ADT
// Written by:
// Date:

#include "myFs.h"

// #include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/// 错误码 -> 错误描述
const char FsErrorMessages[8][64] = {"Fs OK",
                                     "Fs Error",
                                     "File exists",
                                     "No such file or directory",
                                     "Is a directory",
                                     "Not a directory",
                                     "Directory not empty"};

// 此功能应分配和初始化新的 struct FsRep，创建文件系统的根目录，
// 使根目录成为当前的工作目录。然后，它应返回指
// 向分配的 struct FsRep 的指针。
Fs FsNew(void) {
  // 为文件系统分配内存
  Fs fs = malloc(sizeof(struct FsRep));
  assert(fs);
  memset(fs, 0, sizeof(struct FsRep));
  // 初始化根目录
  // 根目录的 parent 是 NULL
  FsInitDir(NULL, &(fs->root), FS_SPLIT_STR);
  // 把 `/../` -> `/`
  fs->root->children[1]->link = fs->root;
  // 初始化当前访问路径
  // 指向根目录
  fs->pathRoot = malloc(sizeof(PATH));
  assert(fs->pathRoot);
  memset(fs->pathRoot, 0, sizeof(PATH));
  fs->current = fs->pathRoot;
  fs->current->file = fs->root;
  return fs;
}

// 这个函数应该在给定的 cwd 数组中存储当前工作目录的规范路径。
// 它可以假设当前工作目录的规范路径不超过
// PATH_MAX 字符。
/// 得到当前目录的路径
/// \param fs
/// \param cwd
void FsGetCwd(Fs fs, char cwd[FS_PATH_MAX + 1]) {
  char *pathAbs = FsPathGetStr(fs->pathRoot);
  strcpy(cwd, pathAbs);
  free(pathAbs);
}

// 这个函数应该释放与给定Fs 关联的所有内存。在处理每个阶段时，
// 您可能需要更新这个函数，以释放您创建的任何新数
// 据结构。
void FsFree(Fs fs) {
  FsFilFree(fs->root);
  free(fs);
}

// 该函数接受一个路径，并在给定文件系统中的该路径上创建一个新目录。
// FsMkdir 执行的功能与Linux 中的mkdir 命令 大致相同。
// 文件已存在于指定路径
// mkdir: cannot create directory 'path': File exists
// 路径的前缀是一个常规文件
// mkdir: cannot create directory 'path': Not a directory
// 路径的正确前缀不存在
// mkdir: cannot create directory 'path': No such file or
// directory
// 错误消息(包括其余函数中的错误消息)都应该打印到标准输出，这意味着应该使用printf
// 打印它们。还要注意，当出现这些错误之一时，程序不应该退出—函数应该简单地返回
// 文件系统，保持不变。
void FsMkdir(Fs fs, char *pathStr) {
  PATH *path = NULL;
  char *pathParentStr = FsPathStrShift(pathStr);
  char *name = FsPathStrGetName(pathStr);
  FsErrors res = FsPathParse(fs->pathRoot, pathParentStr, &path);
  if (res != FS_OK) {
    free(name);
    FsPathFree(path);
    free(pathParentStr);
    PERRORD(res, "mkdir: cannot create directory '%s'", pathStr);
    return;
  }
  PATH *targetPath = FsPathGetTail(path);
  if (targetPath->file->type == REGULAR_FILE) {
    free(name);
    FsPathFree(path);
    free(pathParentStr);
    PERRORD(FS_NOT_A_DIRECTORY, "mkfile: cannot create file '%s'", pathStr);
    return;
  }
  PATH *findingPath = NULL;
  FsErrors resFinding = FsPathParse(fs->pathRoot, pathStr, &findingPath);
  if (resFinding == FS_OK) {
    // 找到了文件，错误。
    PERRORD(FS_FILE_EXISTS, "mkdir: cannot create directory '%s'", pathStr);
  } else {
    // 正常情况
    FIL *dirFile = NULL;
    FsInitDir(targetPath->file, &dirFile, name);
    targetPath->file->children[targetPath->file->size_children++] = dirFile;
    if (targetPath->file->size_children == FS_MAX_CHILDREN) {
      PERROR(FS_ERROR, "Children pool full!");
    }
    FsFilSort(targetPath->file, 0);
  }
  FsPathFree(findingPath);
  FsPathFree(path);
  free(name);
  free(pathParentStr);
}

// 该函数接受一个路径，并在给定文件系统中的该路径上创建一个新的空常规文件。
// 这个函数在Linux 中没有直接等效的命令，但最接近的命令是touch，它可以用来创建空
// 的常规文件，但也有其他用途，如更新时间戳。
FIL *FsMkfile(Fs fs, char *pathStr) {
  PATH *path = NULL;
  char *pathParentStr = FsPathStrShift(pathStr);
  char *name = FsPathStrGetName(pathStr);
  FsErrors res = FsPathParse(fs->pathRoot, pathParentStr, &path);
  if (res != FS_OK) {
    free(name);
    FsPathFree(path);
    free(pathParentStr);
    PERRORD(res, "mkfile: cannot create file '%s'", pathStr);
    return NULL;
  }
  PATH *targetPath = FsPathGetTail(path);
  if (targetPath->file->type == REGULAR_FILE) {
    FsPathFree(path);
    free(name);
    free(pathParentStr);
    PERRORD(FS_NOT_A_DIRECTORY, "mkfile: cannot create file '%s'", pathStr);
    return NULL;
  }
  PATH *findingPath = NULL;
  FsErrors resFinding = FsPathParse(fs->pathRoot, pathStr, &findingPath);
  FIL *file = NULL;
  if (resFinding == FS_OK) {
    // 找到了文件，错误。
    PERRORD(FS_FILE_EXISTS, "mkfile: cannot create directory '%s'", pathStr);
  } else {
    // 正常情况
    file = NULL;
    FsInitFile(targetPath->file, &file, name);
    targetPath->file->children[targetPath->file->size_children++] = file;
    if (targetPath->file->size_children == FS_MAX_CHILDREN) {
      PERROR(FS_ERROR, "Children pool full!");
    }
    FsFilSort(targetPath->file, 0);
  }
  FsPathFree(path);
  free(pathParentStr);
  free(name);
  return file;
}

// 该函数的路径可能为 NULL。
// 如果路径不为 NULL，函数应该将当前工作目录更改为该路径。
// 如果该路径为 NULL，则默认为 root directory (而不是主目录（home directory），
// 因为在这次任务中我们没有主目录)。 该函数大致相当于 Linux 中的 cd 命令。
// 路径的前缀是一个常规文件 cd: 'path': Not a directory
// 路径的前缀不存在 cd: 'path': No such file or directory
void FsCd(Fs fs, char *pathStr) {
  if (!pathStr || !*pathStr) {
    FsPathFree(fs->pathRoot->next);
    fs->pathRoot->file = fs->root;
    fs->pathRoot->next = NULL;
    fs->current = fs->pathRoot;
    return;
  }
  PATH *path = NULL;
  FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
  if (res) {
    PERRORD(res, "cd: '%s'", pathStr);
  } else {
    PATH *pathTail = FsPathGetTail(path);
    if (pathTail->file->type == REGULAR_FILE) {
      PERRORD(FS_NOT_A_DIRECTORY, "cd: '%s'", pathStr);
    } else {
      FsPathFree(fs->pathRoot);
      fs->pathRoot = FsPathClone(path);
      fs->current = FsPathGetTail(fs->pathRoot);
    }
  }

  FsPathFree(path);
}

// 该函数的路径可能为NULL。
// 如果路径不是NULL 并且指向一个目录，那么函数应该打印该目录中所有文件的名称
// (除了. and . . )，按照ASCII 顺序，
// 每行一个。如果路径指向一个文件，那么该函数应该只打印文件系统中存在的给定路径。
// 如果路径为NULL，则默认为当前工作目录。
// 这个函数大致相当于Linux 中的ls 命令。
// 路径的正确前缀(proper prefix)是
// 一个常规文件
// ls: cannot access 'path': Not a directory
// 路径的前缀不存在 ls: cannot access 'path': No such file or
// directory
void FsLs(Fs fs, char *pathStr) {
  FIL *target = NULL;
  if (!pathStr || !*pathStr) {
    target = fs->current->file;
  } else {
    PATH *path = NULL;
    FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
    if (res) {
      PERRORD(res, "ls: cannot access '%s'", pathStr);
      FsPathFree(path);
      return;
    }
    target = FsPathGetTail(path)->file;
    FsPathFree(path);
    if (target->type == REGULAR_FILE) {
      // ls 到一个文件，则输出这个文件~~的绝对路径~~
      // char *pathStrAbs = FsPathGetStr(path);
      // puts(pathStrAbs);
      // free(pathStrAbs);
      // 输出这个文件的输入参数
      puts(pathStr);
      return;
    }
  }
  for (int i = 0; i < target->size_children; i++) {
    FIL *f = target->children[i];
    if (f->link) continue;
#ifdef COLORED
    printf("%s%s%s", (f->type == REGULAR_FILE ? RESET_COLOR : BLUE), f->name,
           RESET_COLOR);
#else
    printf("%s", f->name);
#endif
#ifdef FS_SHOW_DIR_SPLIT
    if (f->type == DIRECTORY)
      printf(FS_SPLIT_STR "\n");
    else
      printf("\n");
#else
    puts("");
#endif
  }
}

// 该函数打印当前工作目录的规范路径。
// 该函数大致相当于 Linux 下的 pwd 命令。
void FsPwd(Fs fs) {
  char *pathStrAbs = FsPathGetStr(fs->pathRoot);
  printf("%s\n", pathStrAbs);
  free(pathStrAbs);
}

// 该函数的路径可能为 NULL。
// 如果路径为 NULL，则默认为根目录。
// 该函数以结构化的方式打印给定路径的目录层次结构(见下面)。
// 这个函数大致相当于 Linux 中的 tree 命令。
// 输出的第一行应该包含给定的路径，如果它存在并指向一个目录。下面的行应该
// 按照 ASCII 顺序显示给定目录下的所有
// 文件，每行一个，用缩进显示哪些文件包含在哪些目录下。
// 每一级缩进增加 4 个空格。请参阅用法示例。
// 路径的前缀是一个常规文件 tree: 'path': Not a directory
// 路径的前缀不存在 tree: 'path': No such file or directory
void FsTree(Fs fs, char *pathStr) {
  if (!pathStr) {
    // 根目录
    FsTree(fs, "/");
    return;
  }
  PATH *path = NULL;
  FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
  if (res) {
    FsPathFree(path);
    PERRORD(res, "tree: '%s'", pathStr);
    return;
  }
  char *pathStrAbs = FsPathGetStr(path);
  PATH *pathTail = FsPathGetTail(path);
  // printf("tree %s:\n", pathStrAbs);
  res = FsTreeInner(pathTail->file, 0, pathStr);
  if (res) {
    FsPathFree(path);
    PERRORD(res, "tree: '%s'", pathStr);
    return;
  }
  FsPathFree(path);
  free(pathStrAbs);
}

// ========== Task 1 ↑ | ↓ Task 2 ==========

// 该函数接受一个路径和一个字符串，并将该路径上的常规文件的内容设置为
// 给定的字符串。如果文件已经有一些内容，那么它将被覆盖。
void FsPut(Fs fs, char *pathStr, char *content) {
  PATH *path = NULL;
  FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
  if (res) {
    PERRORD(res, "put: '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  PATH *pathTail = FsPathGetTail(path);
  if (pathTail->file->type != REGULAR_FILE) {
    PERRORD(FS_IS_A_DIRECTORY, "put: '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  FIL *target = pathTail->file;
  if (target->size_file) {
    if (target->content) free(target->content);
    target->content = NULL;
    target->size_file = 0;
  }
  size_t length = strlen(content) + 1;
  target->content = malloc(sizeof(char) * length);
  memcpy(target->content, content, length);
  target->size_file = length;
  FsPathFree(path);
}

// 该函数接受一个路径，并在该路径上打印常规文件的内容。
// 这个函数大致相当于Linux 中的cat 命令。
void FsCat(Fs fs, char *pathStr) {
  PATH *path = NULL;
  FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
  if (res) {
    PERRORD(res, "put: '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  PATH *pathTail = FsPathGetTail(path);
  if (pathTail->file->type != REGULAR_FILE) {
    PERRORD(FS_IS_A_DIRECTORY, "put: '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  FIL *target = pathTail->file;
  if (target->size_file) {
    printf("%s", target->content);
  }
  FsPathFree(path);
}

// 该函数接受一个指向目录的路径，当且仅当该路径为空时删除该目录。
// 这个函数大致相当于Linux 中的rmdir 命令。
// 为简单起见，可以假设给定路径不包含当前工作目录。
// 注意，这意味着给定的路径永远不会是根目录。如果您愿意(为了
// 完整性起见)，您可以处理这种情况，但是不会对它进行测试。
void FsDldir(Fs fs, char *pathStr) {
  PATH *path = NULL;
  FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
  if (res) {
    PERRORD(res, "dldir: failed to remove '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  PATH *pathTail = FsPathGetTail(path);
  if (pathTail->file->type != DIRECTORY) {
    PERRORD(FS_NOT_A_DIRECTORY, "dldir: failed to remove '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  FIL *dirFile = pathTail->file;
  if (dirFile->size_children > 2) {
    PERRORD(FS_DIRECTORY_NOT_EMPTY, "dldir: failed to remove '%s'", pathStr);
  } else {
    // TODO: check root
    FsFilDlTree(dirFile);
  }
  FsPathFree(path);
}

// 该功能采取路径并删除该路径上的文件。
// 默认情况下，该功能拒绝删除目录：它只会删除目录（及其所有内容递归），如
// 果递归是真实的。如果路径指常规文件，则递归参数无关紧要。
// 此函数大致对应于 Linux 中的 rm 命令，递归真实性与 rm 命令中使用的 -r
// 选项相对应。
void FsDl(Fs fs, bool recursive, char *pathStr) {
  PATH *path = NULL;
  FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
  if (res) {
    PERRORD(res, "dl: cannot remove '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  PATH *pathTail = FsPathGetTail(path);
  FIL *target = pathTail->file;
  if (target->type == DIRECTORY && !recursive) {
    PERRORD(FS_IS_A_DIRECTORY, "dl: failed to remove '%s'", pathStr);
    FsPathFree(path);
    return;
  }
  if (target->type == REGULAR_FILE) {
    FsFilDlTree(target);
  } else {
    if (!recursive && target->size_children > 2) {
      PERRORD(FS_DIRECTORY_NOT_EMPTY, "dl: failed to remove '%s'", pathStr)
    } else {
      FsFilDlTree(target);
    }
  }
  FsPathFree(path);
}

// 该函数接受一个以NULL 结尾的路径数组src 和路径dest。
// 如果src 数组恰好包含一个路径，那么它应该将位于src 的 文件复制到dest。
// 如果src 数组包含多个路径，那么dest 应该指向一个目录，
// 函数应该将src 数组中所有路径下的文 件复制到dest 目录下。
// 默认情况下，函数不复制目录-只有当递归为true 时，它才应该复制目录。
// 这个函数大致相当于Linux 中的cp 命令。
void FsCp(Fs fs, bool recursive, char *src[], char *dest) {
  // TODO: 检查路径包含
  char **pathStrPointer = src;
  if (!*pathStrPointer) {
    PERROR(FS_NO_SUCH_FILE, "cp");
    return;
  }
  PATH *pathDst = NULL;
  FsErrors resDst = FsPathParse(fs->pathRoot, dest, &pathDst);
  if (resDst) {
    if (resDst == FS_NO_SUCH_FILE) {
      // 找不到 Dist 则新建这个文件
      // 取 dst 的上层parent
      PATH *dstPathParent = NULL;
      char *pathParentStr = FsPathStrShift(dest);
      FsErrors res = FsPathParse(fs->pathRoot, pathParentStr, &dstPathParent);
      if (res != FS_OK) {
        PERRORD(res, "cp: '%s'", dest);
      } else {
        PATH *pathParent = NULL;
        res = FsPathParse(fs->pathRoot, *pathStrPointer, &pathParent);
        if (res) {
          PERRORD(res, "cp: '%s'", *pathStrPointer);
        } else {
          PATH *dstPathParentTail = FsPathGetTail(dstPathParent);
          PATH *pathParentTail = FsPathGetTail(pathParent);
          if (pathParentTail->file->type == DIRECTORY && !recursive) {
            PERRORD(FS_IS_A_DIRECTORY, "cp: '%s'", *pathStrPointer);
          } else {
            if (pathParentTail->file->type == DIRECTORY) {
              // 建立对应名字文件夹，然后按文件夹内复制
              FIL *newDir = NULL;
              char *name = FsPathStrGetName(dest);
              FsInitDir(dstPathParentTail->file, &newDir, name);
              free(name);
              for (int i = 0; i < pathParentTail->file->size_children; i++) {
                FsFilCopy(pathParentTail->file->children[i], newDir);
              }
              dstPathParentTail->file
                  ->children[dstPathParentTail->file->size_children++] = newDir;
              FsFilSort(dstPathParentTail->file, 0);
              // FsFilCopy(pathParentTail->file, dstPathParentTail->file);
            } else {
              FIL *newFile = NULL;
              char *name = FsPathStrGetName(dest);
              FsInitFile(dstPathParentTail->file, &newFile, name);
              free(name);
              FsFilCopy(newFile, dstPathParentTail->file);
              FsFilFree(newFile);
            }
          }
        }
        FsPathFree(pathParent);
      }
      free(pathParentStr);
    } else {
      PERRORD(resDst, "cp: '%s'", dest);
    }
    FsPathFree(pathDst);
    return;
  }
  PATH *pathDstTail = FsPathGetTail(pathDst);
  if (pathDstTail->file->type != DIRECTORY) {
    // 目标是个文件，则覆盖这个文件
    // 先删除之
    FIL *dstParent = pathDstTail->file->parent;
    // 只取最上面的文件
    PATH *pathParent = NULL;
    FsErrors res = FsPathParse(fs->pathRoot, *pathStrPointer, &pathParent);
    if (res) {
      PERRORD(res, "cp: '%s'", *pathStrPointer);
      FsPathFree(pathParent);
      return;
    }
    char *nameOld = malloc(sizeof(char) * (pathDstTail->file->name_length + 1));
    strcpy(nameOld, pathDstTail->file->name);
    FsFilDlTree(pathDstTail->file);

    PATH *pathParentTail = FsPathGetTail(pathParent);
    if (pathParentTail->file->link) {
      PERRORD(FS_NO_SUCH_FILE, "cp: '%s'", *pathStrPointer);
    } else {
      // 复制到内存
      FIL *newFile = NULL;
      FsInitFile(pathParentTail->file->parent, &newFile, nameOld);
      newFile->size_file = pathParentTail->file->size_file;
      newFile->content = malloc(sizeof(char) * newFile->size_file);
      memcpy(newFile->content, pathParentTail->file->content,
             sizeof(char) * newFile->size_file);
      // 复制该文件
      if (pathParentTail->file->link) {
        PERRORD(FS_NO_SUCH_FILE, "cp: '%s'", *pathStrPointer);
      } else {
        res = FsFilCopy(newFile, dstParent);
        if (res) {
          PERRORD(res, "cp: '%s'", *pathStrPointer);
        }
      }
      FsFilFree(newFile);
    }
    free(nameOld);
    FsPathFree(pathParent);
  } else {
    // 目标是一个路径
    // 源文件仅包含一个路径
    if (*pathStrPointer && !*(pathStrPointer + 1)) {
      PATH *pathParent = NULL;
      FsErrors res = FsPathParse(fs->pathRoot, *pathStrPointer, &pathParent);
      if (res) {
        PERRORD(res, "cp: '%s'", *pathStrPointer);
        FsPathFree(pathParent);
        return;
      }
      PATH *pathParentTail = FsPathGetTail(pathParent);
      if (pathParentTail->file->link) {
        PERRORD(FS_NO_SUCH_FILE, "cp: '%s'", *pathStrPointer);
      } else {
        if (pathParentTail->file->type == REGULAR_FILE) {
          // 仅仅复制该文件
          if (pathParentTail->file->link) {
            PERRORD(FS_NO_SUCH_FILE, "cp: '%s'", *pathStrPointer);
          } else {
            res = FsFilCopy(pathParentTail->file, pathDstTail->file);
            if (res) {
              PERRORD(res, "cp: '%s'", *pathStrPointer);
            }
          }
        } else {
          // 目标是个文件夹，则复制到文件夹内部
          if (pathDstTail->file->type == DIRECTORY) {
            // 复制文件到该文件夹下
            res = FsFilCopy(pathParentTail->file, pathDstTail->file);
            if (res) {
              PERRORD(res, "cp: '%s'", pathParentTail->file->name);
            }
          } else {
            // 错误，不能 文件夹到文件
            PERRORD(FS_NOT_A_DIRECTORY, "cp: '%s'", *pathStrPointer);
          }
        }
      }
      FsPathFree(pathParent);
    } else {
      // 包含多个路径，则复制这些路径的文件
      while (*pathStrPointer) {
        PATH *path = NULL;
        FsErrors res = FsPathParse(fs->pathRoot, *pathStrPointer, &path);
        if (res) {
          PERRORD(res, "cp: '%s'", *pathStrPointer);
          FsPathFree(path);
          continue;
        }
        PATH *pathTargetTail = FsPathGetTail(path);
        // 不recursive 的时候不复制目录
        if (!recursive && pathTargetTail->file->type == DIRECTORY) {
          PERRORD(FS_IS_A_DIRECTORY, "cp: '%s'", *pathStrPointer);
        } else {
          FsFilCopy(pathTargetTail->file, pathDstTail->file);
        }
        FsPathFree(path);
        pathStrPointer++;
      }
    }
  }

  FsPathFree(pathDst);
}

// 该函数接受以null 结尾的src 路径数组和dest 路径。
// 它应该将src 中所有路径所指向的文件移动到dest。
// 该函数大致相当于Linux 中的mv 命令。
void FsMv(Fs fs, char *src[], char *dest) {
  // TODO: 检查路径包含
  char **pathStrPointer = src;
  if (!*pathStrPointer) {
    PERROR(FS_NO_SUCH_FILE, "mv");
    return;
  }
  PATH *pathDst = NULL;
  FsErrors resDst = FsPathParse(fs->pathRoot, dest, &pathDst);
  if (resDst) {
    if (resDst == FS_NO_SUCH_FILE) {
      // 找不到 dist 则新建文件
      // 取 dst 的上层parent
      PATH *dstPathParent = NULL;
      char *pathParentStr = FsPathStrShift(dest);
      FsErrors res = FsPathParse(fs->pathRoot, pathParentStr, &dstPathParent);
      if (res != FS_OK) {
        PERRORD(res, "mv: '%s'", dest);
      } else {
        PATH *pathParent = NULL;
        res = FsPathParse(fs->pathRoot, *pathStrPointer, &pathParent);
        if (res) {
          PERRORD(res, "mv: '%s'", *pathStrPointer);
        } else {
          // 改名字然后移动
          PATH *dstPathParentTail = FsPathGetTail(dstPathParent);
          PATH *pathParentTail = FsPathGetTail(pathParent);
          free(pathParentTail->file->name);
          char *name = FsPathStrGetName(dest);
          size_t length = strlen(name);
          pathParentTail->file->name = malloc(sizeof(char) * (length + 1));
          strcpy(pathParentTail->file->name, name);
          pathParentTail->file->name_length = length;
          free(name);
          res = FsFilMove(pathParentTail->file, dstPathParentTail->file);
          if (res) {
            PERRORD(res, "mv: '%s'", dest);
          }
        }
        FsPathFree(pathParent);
      }
      free(pathParentStr);
    } else {
      PERRORD(resDst, "mv: '%s'", dest);
    }
    FsPathFree(pathDst);
    return;
  }
  PATH *pathDstTail = FsPathGetTail(pathDst);
  if (pathDstTail->file->type != DIRECTORY) {
    // 目标是个文件，则覆盖这个文件
    // 先删除之
    FIL *dstParent = pathDstTail->file->parent;
    // 只取最上面的文件
    PATH *pathParent = NULL;
    FsErrors res = FsPathParse(fs->pathRoot, *pathStrPointer, &pathParent);
    if (res) {
      PERRORD(res, "mv: '%s'", *pathStrPointer);
      FsPathFree(pathParent);
      return;
    }
    FsFilDlTree(pathDstTail->file);

    PATH *pathParentTail = FsPathGetTail(pathParent);
    // 移动到目标处
    FsFilMove(pathParentTail->file, dstParent);
    FsPathFree(pathParent);
  } else {
    // 移动这些路径的文件
    while (*pathStrPointer) {
      PATH *path = NULL;
      FsErrors res = FsPathParse(fs->pathRoot, *pathStrPointer, &path);
      if (res) {
        PERRORD(res, "mv: '%s'", *pathStrPointer);
        FsPathFree(path);
        continue;
      }
      PATH *pathTargetTail = FsPathGetTail(path);
      FsFilMove(pathTargetTail->file, pathDstTail->file);
      FsPathFree(path);
      pathStrPointer++;
    }
  }
  FsPathFree(pathDst);
}

extern Fs fs;

FIL *FsFilFindByFile(size_t file) { return files_list[file]; }

size_t fno_read_myfs(void *buf, size_t size, size_t n, int file,
                     size_t *offset) {
  FIL *target = FsFilFindByFile(file);
  if (!offset) offset = &target->offset;
  // Log("offset = %d, size=%d, n=%d", (int)*offset, (int)size, (int)n);
  size_t res = target->size_file / size - *offset;
  res = res > n ? n : res;
  FsRead(target, *offset, size * res, buf);
  // Log("res = %d, filesize/size = %d", (int)res, (int)(target->size_file / size));
  *offset += res * size;
  if (offset != &target->offset) target->offset = *offset;
  return res;
}

size_t fread_myfs(void *buf, size_t size, size_t n, FILE *f) {
  return fno_read_myfs(buf, size, n, get_file_no(f), (size_t *)&f->_offset);
}

size_t fseek_myfs(FILE *f, size_t offset, size_t direction) {
  FIL *target = FsFilFindByFile(get_file_no(f));
  f->_offset = offset < target->size_file ? offset : target->size_file;
  return 0;
}

int _read_myfs(int file, void *ptr, size_t len) {
  Log("_read_myfs file=%d", file);
  Log("fno_read_myfs(ptr, 1, len, file=%d, NULL)", file);
  int l = fno_read_myfs(ptr, 1, len, file, NULL);
  return len - l;
}

FILE *fopen_myfs(const char *pathStr, const char *method) {
  FILE *f = (FILE *)malloc(sizeof(FILE));
  memset(f, 0, sizeof(*f));

  PATH *path = NULL;
  FsErrors res = FsPathParse(fs->pathRoot, pathStr, &path);
  if (res) {
    PERRORD(res, "open: '%s'", pathStr);
    FsPathFree(path);
    free(f);
    return NULL;
  }
  PATH *pathTail = FsPathGetTail(path);
  if (pathTail->file->type != REGULAR_FILE) {
    PERRORD(FS_IS_A_DIRECTORY, "open: '%s'", pathStr);
    FsPathFree(path);
    free(f);
    return NULL;
  }
  FIL *target = pathTail->file;
  get_file_no(f) = target->file;
  // f->_read = _read_myfs;

  FsPathFree(path);
  return f;
}

int fclose_myfs(FILE *f) {
  free(f);
  return 0;
}