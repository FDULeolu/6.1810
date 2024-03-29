#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void child_process(int read_fd, int write_fd, int pid) {
    char byte = 0;
    read(read_fd, &byte, sizeof(byte));
    printf("%d: received ping\n", pid);
    write(write_fd, &byte, sizeof(byte));
    exit(0);
}

int main() {
    int pipe1[2], pipe2[2];
    int pid;
    char* byte;

    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        fprintf(2, "Pipe creation failed\n");
        exit(1);
    }

    pid = fork();

    if (pid == -1) {
        fprintf(2, "Fork failed\n");
        exit(1);
    } else if (pid == 0) {
        // Child process
        close(pipe1[1]);    // close the write end of pipe1
        close(pipe2[0]);    // close the read end of pipe2
        child_process(pipe1[0], pipe2[1], pid);
    } else {
        close(pipe1[0]);
        close(pipe2[1]);
        write(pipe1[1], &byte, sizeof(byte));
        read(pipe2[0], &byte, sizeof(byte));
        printf("%d: received pong\n", pid);
        exit(0);
    }
}