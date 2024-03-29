#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("xargs: invalid input");
        exit(1);
    }
    char buf[64];
    char *p = buf;
    char word;
    int argv_count = 0;
    char *xargv_argv[32];
    for (int i = 1; i < argc; i++) {
        xargv_argv[argv_count] = (char*) malloc(strlen(argv[i]) + 1);
        strcpy(xargv_argv[argv_count], argv[i]);
        // DEBUG
        // printf("%s\n", xargv_argv[argv_count]);
        // DEBUG
        argv_count++;
    }
    while (read(0, &word, sizeof(char))) {
        if (word != '\n') {
            *p = word;
            p++;
        } else {
            *p = '\0';
            xargv_argv[argv_count] = (char*) malloc(strlen(buf) + 1);
            strcpy(xargv_argv[argv_count], buf);
            p = buf;
            // DEBUG
            // printf("%s", buf);
            // DEBUG
            argv_count++;
        }
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed");
        exit(1);
    } else if (pid == 0) {
        exec(argv[1], xargv_argv);
    } else {
        wait(0);
        for (int i = 0; i < argv_count; i++) {
            free(xargv_argv[i]);
        }
        exit(0);
    }
}
