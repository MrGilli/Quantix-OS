#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "fs.h"

#define BRAND_QEMU  1
#define BRAND_VBOX  2

#define MAX_FILENAME_LENGTH 20
#define MAX_FILE_COUNT 100
#define MAX_FILE_CONTENT_LENGTH 10000

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    int size;
    char content[MAX_FILE_CONTENT_LENGTH];
} File;

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    int file_count;
    File files[MAX_FILE_COUNT];
} Directory;

Directory root_directory;

void custom_strcpy(char *dest, const char *src) {
    while (*src != '\0') {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0'; // Null-terminate the destination string
}

void initFileSystem() {
    custom_strcpy(root_directory.name, "root");
    root_directory.file_count = 0;
}

void createFile(char *name, char *content) {
    if (root_directory.file_count < MAX_FILE_COUNT) {
        File newFile;
        custom_strcpy(newFile.name, name);
        newFile.size = strlen(content);
        custom_strcpy(newFile.content, content);
        newFile.content[MAX_FILE_CONTENT_LENGTH - 1] = '\0';

        root_directory.files[root_directory.file_count++] = newFile;
        printf("File '%s' created successfully.\n", name);
    } else {
        printf("File system full. Cannot create more files.\n");
    }
}

void listFiles() {
    printf("Files in root directory:\n");
    for (int i = 0; i < root_directory.file_count; ++i) {
        printf("- %s, %s\n", root_directory.files[i].name, root_directory.files[i].content);
    }
}
