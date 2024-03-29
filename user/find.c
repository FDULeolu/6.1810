#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* get_file_name(char *path) {
    char *name = malloc(DIRSIZ);
    char *p = path + strlen(path);
    while (*p != '/' && p >= path) { p--; }
    if (p > path) {
        p++;
    }
    strcpy(name, p);
    return name;
}


void find(char *path, char *file_name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    char *name;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot access this file");
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat this path");
        close(fd);
        return;
    }

    switch (st.type) {
        case T_DEVICE:
        case T_FILE:
            name = get_file_name(path);
            if (strcmp(name, file_name) == 0) {
                printf("%s\n", path);
                free(name);
            }
            break;
        
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) {
                    continue;
                } else if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if (st.type == T_FILE && strcmp(get_file_name(buf), file_name) == 0) {
                    printf("%s\n", buf);
                } else if (st.type == T_DIR) {
                    find(buf, file_name);
                }
            }
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "find: invalid input, valid format is find [path] [filename]");
        exit(1);
    } else if (argc == 2) {
        find(".", argv[1]);
    } else if (argc == 3) {
        find(argv[1], argv[2]);
    } else {
        fprintf(2, "find: invalid input, valid format is find [path] [filename]");
        exit(1);
    }
    exit(0);
}