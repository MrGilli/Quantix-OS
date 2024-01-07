#ifndef FS_H
#define FS_H

#include <string.h>
#include <stdint.h>

//FUNCS
void custom_strcpy(char *dest, const char *src);
void initFileSystem();
void createFile(char *name, char *content);
void listFiles();


//CONST
#define BRAND_QEMU  1
#define BRAND_VBOX  2

#define MAX_FILENAME_LENGTH 20
#define MAX_FILE_COUNT 100
#define MAX_FILE_CONTENT_LENGTH 10000

#endif
