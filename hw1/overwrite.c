#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

enum args {
    FILENAME = 1,
    OFFSET,
    DATA,
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        puts("It needs 3 arguments");
        puts("Usage: ./a.out filename offset data"); return 1;
    }

    int fd;
    fd = open(argv[FILENAME], O_RDWR | O_CREAT, 0644);

    lseek(fd, (off_t)atol(argv[OFFSET]), SEEK_SET);

    write(fd, argv[DATA], strlen(argv[DATA]));

    if (close(fd) == -1) {
        perror("Error: ");
    }

}