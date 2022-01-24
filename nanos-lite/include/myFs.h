// Interface to the File System ADT

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef BASH_FS_H
#define BASH_FS_H

#include "utility.h"

#define FS_PATH_MAX 4096

typedef struct FsRep *Fs;

Fs FsNew(void);

void FsGetCwd(Fs fs, char cwd[FS_PATH_MAX + 1]);

void FsFree(Fs fs);

void FsMkdir(Fs fs, char *path);

FIL *FsMkfile(Fs fs, char *path);

void FsCd(Fs fs, char *path);

void FsLs(Fs fs, char *path);

void FsPwd(Fs fs);

void FsTree(Fs fs, char *path);

void FsPut(Fs fs, char *pathStr, char *content);

void FsCat(Fs fs, char *path);

void FsDldir(Fs fs, char *path);

void FsDl(Fs fs, bool recursive, char *path);

void FsCp(Fs fs, bool recursive, char *src[], char *dest);

void FsMv(Fs fs, char *src[], char *dest);

size_t fno_read_myfs(void *buf, size_t size, size_t n, int file, size_t *offset);

size_t fread_myfs(void *buf, size_t size, size_t n, FILE *f);

size_t fseek_myfs(FILE *f, size_t offset, size_t direction);

FILE *fopen_myfs(const char *pathStr, const char *method);

int fclose_myfs(FILE *f);

#ifndef __ARCH_NATIVE
#define get_file_no(f) (f->_file)
#else
#define get_file_no(f) (f->_fileno)
#endif

#endif
