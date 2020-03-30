#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
extern int errno;

#define BUF_SIZE 100
#define OVERFLOW_PADDING 8
#define SRC_FILENAME 1
#define TAR_FILENAME 2

int main(int argc, char* argv[]) {
    if (argc != 3) {
        puts("It needs 2 arguments");
        puts("Usage: ./a.out source_filename target_filename"); return 1;
    }

    char buf[BUF_SIZE + OVERFLOW_PADDING];
    int src_fd, tar_fd;
    src_fd = open(argv[SRC_FILENAME], O_RDONLY);
    tar_fd = open(argv[TAR_FILENAME], O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (src_fd == -1) {
        printf("Error Number %d\n", errno);
        perror("Error: ");
    }

    if (tar_fd == -1) {
        printf("Error Number %d\n", errno);
        perror("Error: ");
    }

    lseek(src_fd, 0, SEEK_SET);
    lseek(tar_fd, 0, SEEK_SET);

    size_t size = 0;
    while ((size = read(src_fd, buf, BUF_SIZE)) > 0) {
        write (tar_fd, buf, size);
    }

    if (close(src_fd) ==  -1) {
        perror("Error: ");
    }
    if (close(tar_fd) == -1) {
        perror("Error: ");
    }

    return 0;
}
