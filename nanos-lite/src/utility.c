// Written by:
// Date:

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myFs.h"

/// 从文件名查找文件夹内的文件
/// \param dir
/// \param name
/// \return
FIL *FsFilFindByName(FIL *dir, const char *name) {
  // TODO: 优化查找算法
  // 先用线性查找
  for (size_t i = 0; i < dir->size_children; i++) {
    if (strcmp(dir->children[i]->name, name) == 0) {
      return dir->children[i];
    }
  }
  // 找不到文件
  return NULL;
}

/// 按照字典序排序文件夹中的文件排列顺序
/// \param dir
/// \param reverse
void FsFilSort(FIL *dir, int reverse) {
  if (!dir || dir->size_children == 0 || dir->type == REGULAR_FILE || dir->link)
    return;
  FIL *tmp = NULL;
  // 使用简单的冒泡排序
  for (int i = 0; i < dir->size_children; i++) {
    if (dir->children[i]->link) continue;
    for (int j = i + 1; j < dir->size_children; j++) {
      if (dir->children[j]->link) continue;
      if ((strcmp(dir->children[i]->name, dir->children[j]->name) *
           (reverse ? -1 : 1)) > 0) {
        // printf("%s <==> %s\n", dir->children[i]->name,
        // dir->children[j]->name);
        tmp = dir->children[i];
        dir->children[i] = dir->children[j];
        dir->children[j] = tmp;
      }
    }
  }
}

/// DEBUG: 打印一个文件的信息
/// \param file
void FsFilPrint(FIL *file) {
  if (!file) {
    printf("[EMPTY FILE]\n");
    return;
  }
  if (file->link) {
    printf("[link] %s -> %s\n", file->name, file->link->name);
    return;
  }
  if (file->type == DIRECTORY) {
    printf("[dir ] %s: ", file->name);
    FsFilSort(file, 0);
    for (int i = 0; i < file->size_children; i++) {
      printf("%s%s", file->children[i]->name,
             i + 1 == file->size_children ? "\n" : ", ");
    }
  } else {
    printf("[file] %s\n", file->name);
  }
}

/// 获取路径末尾
/// \param path
/// \return
PATH *FsPathGetTail(PATH *path) {
  while (path->next) path = path->next;
  return path;
}

/// 清理文件内存
/// \param file
void FsFilFree(FIL *file) {
  if (!file) return;
  if (file->link) {
    // Link 文件只释放本身
    free(file);
    return;
  }
  if (file->type == DIRECTORY) {
    for (int i = 0; i < file->size_children; i++) {
      FsFilFree(file->children[i]);
    }
    free(file->children);
  } else {
    if (file->content) free(file->content);
  }
  free(file->name);
  free(file);
}

/// 清理路径内存
/// \param path
void FsPathFree(PATH *path) {
  if (!path) return;
  FsPathFree(path->next);
  free(path);
}

/// 在 PATH 链表后插入一个 file
/// \param tail
/// \param file
/// \return 插入后的 path
PATH *FsPathInsert(PATH *tail, FIL *file) {
  assert(tail);
  assert(file);
  tail->next = malloc(sizeof(PATH));
  assert(tail->next);
  memset(tail->next, 0, sizeof(PATH));
  tail->next->file = file;
  tail->next->forward = tail;
  return tail->next;
}

size_t file_tail = MYFS_FILE_START;
FIL *files_list[MYFS_FILE_MAX];

/// 初始化一个文件(FIL)
/// \param parent
/// \param file
/// \param name
void FsFilInit(FIL *parent, FIL **file, const char *name) {
  if (!name) return;
  // 分配储存内存
  *file = (FIL *)malloc(sizeof(FIL));
  assert(*file);
  // 初始化内存
  memset(*file, 0, sizeof(FIL));
  // 分配文件名字内存空间，并且复制名字内容
  // 注意文件名字包含最后结束符\\0，所以多分配一个字节
  (*file)->name_length = strlen(name);
  (*file)->name = (char *)malloc(sizeof(char) * ((*file)->name_length + 1));
  assert((*file)->name);
  strcpy((*file)->name, name);
  (*file)->parent = parent;
  (*file)->type = REGULAR_FILE;
  (*file)->file = file_tail;
  files_list[file_tail++] = *file;
}

/// 新建文件链接
/// \param parent
/// \param name
void FsMkLink(FIL *parent, FIL *link_to, const char *name) {
  FIL *file = NULL;
  FsFilInit(parent, &file, name);
  file->link = link_to;
  file->type = DIRECTORY;
  parent->children[parent->size_children++] = file;
  if (parent->size_children == FS_MAX_CHILDREN) {
    PERROR(FS_ERROR, "Children pool full!")
  }
}

/// 初始化文件夹结构
/// \param file
void FsInitDir(FIL *parent, FIL **file, const char *name) {
  FsFilInit(parent, file, name);
  (*file)->type = DIRECTORY;
  // 初始化文件列表空间
  (*file)->children = malloc(sizeof(FIL *) * FS_MAX_CHILDREN);
  assert((*file)->children);
  memset((*file)->children, 0, sizeof(FIL *) * FS_MAX_CHILDREN);
  // 新建两个文件夹：.和..，指向自己或者上层
  FsMkLink(*file, *file, ".");
  FsMkLink(*file, parent, "..");
  // 关于"."和".."文件夹：
  // 1. FileType 为 目录
  // 2. name == "." or ".."
  // 3. size_children == 0 and children == NULL
  // 4. link == parent or self
}

/// 初始化文件结构
/// \param parent
/// \param file
/// \param name
void FsInitFile(FIL *parent, FIL **file, const char *name) {
  FsFilInit(parent, file, name);
  (*file)->type = REGULAR_FILE;
}

/// 复制路径结构
/// \param src
/// \param dst
PATH *FsPathClone(PATH *src) {
  PATH *dst = malloc(sizeof(PATH));
  assert(dst);
  memset(dst, 0, sizeof(PATH));
  PATH *p = dst;
  PATH *s = src;
  p->file = s->file;
  while (s->next) {
    p = FsPathInsert(p, s->next->file);
    s = s->next;
  }
  return dst;
}

/// 路径 -> 绝对路径字符串
/// \param path
/// \return
char *FsPathGetStr(PATH *path) {
  size_t length = 0;
  PATH *p = path;
  while (p) {
    length += p->file->name_length + 1;
    p = p->next;
  }
  char *pathStr = malloc(sizeof(char) * (length + 1));
  char *str = pathStr;
  p = path;
  while (p) {
    strcpy(str, p->file->name);
    str += p->file->name_length;
    if (strcmp(p->file->name, FS_SPLIT_STR) != 0 &&
        p->file->type == DIRECTORY && p->next) {
      *(str++) = FS_SPLIT;
    }
    p = p->next;
  }
  *str = '\0';
  return pathStr;
}

/// 简化路径，`/a/../b` -> `/b`
/// \param path
void FsPathSimplify(PATH **path) {
  // 清理 Path 中的 link 链接类型
  PATH *p = *path;
  if (!p) return;
  // Path 最顶层一定不是 link
  while (p && p->next) {
    if (p->next->file->link) {
      // 遇到 link，则开始收缩
      if (p->next->file->link == p->file &&
          p->next->file->link != (*path)->file) {
        // "." 路径
        // 直接跳过这个 Node
        PATH *tmp = p->next;
        tmp->next = NULL;
        p->next = p->next->next;
        FsPathFree(tmp);
      } else {
        // ".." 路径
        // 跳过 p、.. 两个 Node
        PATH *tmp = p->forward;
        if (p->next) {
          if (!p->forward) {
            // p: `/`
            // `/..` -> `/`
            FsPathFree(p->next);
            p->next = NULL;
            break;
          } else {
            p->forward->next = p->next->next;
          }
          p->next->next = NULL;
          FsPathFree(p->next);
        } else
          p->forward->next = p->next;
        p->next = NULL;
        FsPathFree(p);
        p = tmp;
      }
    }
    if (!p) break;
    p = p->next;
  }
}

/// 解析路径字符串到路径结构
/// \param pathRoot
/// \param pathStr
/// \param path
/// \return
FsErrors FsPathParse(PATH *pathRoot, const char *pathStr, PATH **path) {
  if (!path) return FS_ERROR;
  const char *p = pathStr;
  if (!pathStr || !*pathStr) {
    // 空串，返回 pwd
    *path = FsPathClone(pathRoot);
    return FS_OK;
  }
  if (*p != FS_SPLIT) {
    // 不以'/'开头，是相对目录
    if (!pathRoot) return FS_ERROR;
    // 就先转换为绝对目录
    // 复制路径结构然后简化路径
    *path = FsPathClone(pathRoot);
  } else {
    *path = malloc(sizeof(PATH));
    assert(*path);
    memset(*path, 0, sizeof(PATH));
    (*path)->file = pathRoot->file;
  }
  // 去除开头连续的 `//`
  while (*pathStr == FS_SPLIT && *(pathStr + 1) == FS_SPLIT) pathStr++;
  // 现在 path 已经是绝对路径，接下来拼接 pathStr，
  PATH *pathTail = FsPathGetTail(*path);
  // 拼接过程中检查文件是否存在
  char buf[FS_PATH_MAX];
  char *p2 = NULL;
  // 特殊处理 '/' 开头：去掉
  while (*p == FS_SPLIT) p++;
  while (*p) {
    // 遇到'/' || 是相对路径开头 || 是绝对路径的'/'
    if (*p == FS_SPLIT || (*pathStr != FS_SPLIT && p == pathStr) ||
        (*pathStr == FS_SPLIT && p == pathStr + 1)) {
      while (*p == FS_SPLIT && *p) p++;
      if (!*p) return FS_OK;
      // 向下加一层文件夹
      // 找到文件名
      p2 = buf;
      while (*p != FS_SPLIT && *p) *(p2++) = *(p++);
      *p2 = '\0';
      // 查找对应文件是否存在
      FIL *target = FsFilFindByName(pathTail->file, buf);
      if (!target) {
        return FS_NO_SUCH_FILE;
      }
      // path 尾部处理：判断有无 '/' 结尾
      if (target->type == REGULAR_FILE) {
        // 找到文件的话，如果 pathStr 后面已经没有东西了
        // 那么就是正确的，否则是错误 FS_NOT_A_DIRECTORY
        if ((*p == FS_SPLIT && *(p + 1) == '\0') || *p == '\0') {
          pathTail = FsPathInsert(pathTail, target);
          FsPathSimplify(path);
          pathTail = FsPathGetTail(*path);
          return FS_OK;
        } else {
          return FS_NOT_A_DIRECTORY;
        }
      } else {
        // target->type == DIRECTORY
        pathTail = FsPathInsert(pathTail, target);
        FsPathSimplify(path);
        pathTail = FsPathGetTail(*path);
      }
    }
    if (!*p) break;
    // p++;
  }
  return FS_OK;
}

/// 缩减路径得到文件名
/// \param pathStr
/// \return
char *FsPathStrGetName(char *pathStr) {
  size_t length = strlen(pathStr);
  char *newName = malloc(sizeof(char) * (length + 1));
  strcpy(newName, pathStr);
  char *p = newName + length - 1;
  while (*p == '/' && *p) {
    *p = '\0';
    p--;
  }
  while (*p && p >= newName) {
    if (*p == FS_SPLIT && p != newName + length - 1) {
      size_t length2 = strlen(p + 1);
      char *newNewName = malloc(sizeof(char) * (length2 + 1));
      strcpy(newNewName, p + 1);
      free(newName);
      return newNewName;
    }
    p--;
  }
  if (!*p) {
    // return p;
    // return "";
    *p = '\0';
  }
  return newName;
}

/// 复制一份 pathStr，并且切断到最后一个 '/' 之前
/// \param pathStr
/// \return
char *FsPathStrShift(char *pathStr) {
  size_t length = strlen(pathStr);
  char *dst = malloc(sizeof(char) * (length + 1));
  strcpy(dst, pathStr);
  char *p = dst + length - 1;
  while (*p && p > dst) {
    if (*p == FS_SPLIT && p != dst + length - 1) {
      *(p + 1) = '\0';
      return dst;
    }
    p--;
  }
  if (p == dst) *p = '\0';
  return dst;
}

/// FsTree 的内层循环
/// \param file
/// \param layer
/// \param pathStrInput
/// \return
FsErrors FsTreeInner(FIL *file, int layer, char *pathStrInput) {
  if (!file) return FS_OK;
  // printf("Tree: [%d] %s\n", layer, file->name);
  if (file->type == REGULAR_FILE) {
    return FS_NOT_A_DIRECTORY;
  }
  // printf("FsTreeInner: %s\n", file->name);
  // FsFilPrint(file);
  FsFilSort(file, 0);
  if (layer == 0)
    // printf("%s\n", file->name);
    printf("%s\n", pathStrInput);
  for (size_t i = 0; i < file->size_children; i++) {
    FIL *f = file->children[i];
    if (f->link) continue;
    for (int j = 0; j < layer + 1; j++) printf("    ");
#ifdef COLORED
    printf("%s%s%s", (f->type == REGULAR_FILE ? RESET_COLOR : BLUE), f->name,
           RESET_COLOR);
#else
    printf("%s", f->name);
#endif
#ifdef FS_SHOW_DIR_SPLIT
    if (f->type == DIRECTORY)
      puts(FS_SPLIT_STR);
    else
      puts("");
#else
    puts("");
#endif
    if (f->type == DIRECTORY) {
      FsErrors res = FsTreeInner(f, layer + 1, pathStrInput);
      if (res) {
        return res;
      }
    }
  }
  return FS_OK;
}

/// 删除文件树
/// \param file
void FsFilDlTree(FIL *file) {
  FIL *parent = file->parent;
  int found = -1;
  for (int i = 0; i < parent->size_children; i++) {
    FIL *f = parent->children[i];
    if (f == file) {
      found = i;
      break;
    }
  }
  if (found < 0) {
    PERROR(FS_ERROR, "Internal Error!");
    exit(1);
  }
  FsFilFree(file);
  if (parent->size_children == 1) {
    parent->size_children = 0;
  } else {
    parent->children[found] = parent->children[parent->size_children - 1];
    parent->size_children--;
    FsFilSort(parent, 0);
  }
}

/// 复制文件结构信息
/// \param src
/// \param dst
/// \return
FsErrors FsFilCopy(FIL *src, FIL *dst) {
  if (!src || !dst) return FS_ERROR;
  if (src->link) return FS_OK;
  if (dst->type != DIRECTORY) {
    return FS_NOT_A_DIRECTORY;
  }
  // 检查是否有同名文件
  FIL *found = FsFilFindByName(dst, src->name);
  if (found) {
    // 有同名文件，先删除
    // return FS_FILE_EXISTS;
    FsFilDlTree(found);
  }
  FIL *data = NULL;
  if (src->type == DIRECTORY) {
    FsInitDir(dst, &data, src->name);
  } else {
    FsInitFile(dst, &data, src->name);
  }
  dst->children[dst->size_children++] = data;
  // 整理顺序
  FsFilSort(dst, 0);
  // 默认递归复制
  if (src->type == DIRECTORY) {
    for (int i = 0; i < src->size_children; i++) {
      FsFilCopy(src->children[i], data);
    }
  } else {
    data->size_file = src->size_file;
    data->content = malloc(sizeof(char) * src->size_file);
    memcpy(data->content, src->content, src->size_file);
  }
  return FS_OK;
}

/// 按文件结构信息移动
/// \param src
/// \param dst
/// \return
FsErrors FsFilMove(FIL *src, FIL *dst) {
  if (!src || !dst) return FS_ERROR;
  if (dst->type != DIRECTORY) return FS_NOT_A_DIRECTORY;
  int parentIndex = -1;
  for (int i = 0; i < src->parent->size_children; i++) {
    if (src->parent->children[i] == src) {
      parentIndex = i;
      break;
    }
  }
  if (parentIndex < 0) return FS_ERROR;
  if (src->parent->size_children == 1) {
    src->parent->size_children = 0;
  } else {
    src->parent->children[parentIndex] =
        src->parent->children[--src->parent->size_children];
    FsFilSort(src->parent, 0);
  }
  src->parent = dst;
  dst->children[dst->size_children++] = src;
  FsFilSort(dst, 0);
  return FS_OK;
}

/// DEBUG: 打印文件信息
/// \param fs
/// \param pathStr
void FsPrint(Fs fs, char *pathStr) {
  PATH *path = NULL;
  FsPathParse(fs->pathRoot, pathStr, &path);
  FsFilPrint(FsPathGetTail(path)->file);
  FsPathFree(path);
}

int FsWrite(FIL *target, size_t size, void *src, int mode) {
  if (mode == FS_WRITE_NEW) {
    if (target->size_file) {
      if (target->content) free(target->content);
      target->content = NULL;
      target->size_file = 0;
    }
    target->content = malloc(sizeof(char) * size);
    memcpy(target->content, src, size);
    target->size_file = size;
  } else if (mode == FS_WRITE_APPEND) {
    if (!target->size_file) {
      return FsWrite(target, size, src, FS_WRITE_NEW);
    }
    char *origin = target->content;
    size_t origin_size = target->size_file;
    target->content = NULL;
    target->size_file += size;
    target->content = malloc(sizeof(char) * target->size_file);
    memcpy(target->content, origin, origin_size);
    memcpy(target->content + origin_size, src, size);
    free(origin);
  }
  return 0;
}

int FsRead(FIL *target, size_t offset, size_t size, void *dst) {
  memcpy(dst, target->content + offset, size);
  return 0;
}