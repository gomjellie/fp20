#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

enum args {
    FILENAME = 1,
    OFFSET,
    READ_BYTE,
};

int main(int argc, char *argv[]) {
    int fd;
    char c;
    long int cnt = 0;
    if (argc != 4) {
        puts("It needs 3 arguments");
        puts("Usage: ./a.out filename offset bytes_to_read"); return 1;
    }

    fd = open(argv[FILENAME], O_RDONLY);

    if (fd == -1) {
        puts("Error opening file"); return 1;
    }

    lseek(fd, (off_t)atol(argv[OFFSET]), SEEK_SET);

    while (read(fd, &c, 1) != 0 && cnt < atol(argv[READ_BYTE])) {
        cnt ++;
        write(1, &c, 1);
    }

    if (close(fd) == -1) perror("Error: ");

    return 0;
}