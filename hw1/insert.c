#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUF_SIZE 128
#define BUF_PADD 8

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

    int fd, backup_fd;
    fd = open(argv[FILENAME], O_RDWR | O_CREAT, 0644);
    backup_fd = open("backup", O_RDWR | O_CREAT | O_TRUNC, 0644);

    lseek(fd, (off_t)atol(argv[OFFSET]), SEEK_SET);

    char buff[BUF_SIZE + BUF_PADD];

    size_t read_len = 0;
    while ((read_len = read(fd, buff, BUF_SIZE)) > 0)
        write(backup_fd, buff, read_len); // backup to backup_fd
    
    lseek(fd, (off_t)atol(argv[OFFSET]), SEEK_SET);

    write(fd, argv[DATA], strlen(argv[DATA]));

    lseek(backup_fd, 0, SEEK_SET);
    while ((read_len = read(backup_fd, buff, BUF_SIZE)) > 0)
        write(fd, buff, read_len); // restore from backup_fd

    if (close(fd) == -1) perror("Error: ");
    if (close(backup_fd) == -1) perror("Error: ");
    if (remove("backup") == -1) perror("Error: ");

    return 0;
}