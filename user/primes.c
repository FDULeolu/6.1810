#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1

#define STDERR 2

int main() {
    int numbers[36], pipe0[2];
    int pid, index = 0;

    for (int i = 2; i < 36; i++) {
        numbers[index++] = i;
    }

    while (index > 0) {
        pipe(pipe0);
        pid = fork();
        if (pid < 0) {
            fprintf(STDERR, "fork error");
        } else if (pid > 0) {
            close(pipe0[READEND]);
            for (int i = 0; i < index; i++) {
                write(pipe0[WRITEEND], &numbers[i], sizeof(numbers[i]));
            }
            close(pipe0[WRITEEND]);
            wait(0);
            exit(0);
        } else if (pid == 0) {
            close(pipe0[WRITEEND]);
            int prime = 0, temp = 0;
            index = -1;

            while (read(pipe0[READEND], &temp, sizeof(temp)) != 0) {
                if (index < 0) {
                    prime = temp;
                    printf("prime %d\n", prime);
                }
                else {
                    if (temp % prime != 0) {
                        numbers[index++] = temp;
                    }
                }
            }
            close(pipe0[READEND]);
        }
    }
    exit(0);
}

