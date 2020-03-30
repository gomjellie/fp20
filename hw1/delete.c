#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUF_SIZE 128
#define BUF_PADD 8

enum args {
    FILENAME = 1,
    OFFSET,
    DELETE_BYTE,
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        puts("It needs 3 arguments");
        puts("Usage: ./a.out filename offset bytes_to_delete"); return 1;
    }

    int lfd, ofd, rfd; // left, original, right
    char buf[BUF_SIZE + BUF_PADD];
    size_t read_len = 0; long int len_cnt = 0;

    ofd = open(argv[FILENAME], O_RDONLY);
    lfd = open("left", O_RDWR | O_CREAT | O_TRUNC, 0644);
    rfd = open("right", O_RDWR | O_CREAT | O_TRUNC, 0644);
    
    while ((read(ofd, buf, 1)) > 0 &&
        len_cnt < atol(argv[OFFSET])
    ) {
        len_cnt ++;
        write(lfd, buf, 1);
    }

    lseek(ofd, (off_t)(atol(argv[DELETE_BYTE]) - 1L), SEEK_CUR);

    while ( (read_len = read(ofd, buf, BUF_SIZE)) > 0 )
        write(rfd, buf, read_len);
    
    if (close(ofd) == -1) perror("Error: ");
    ofd = open(argv[FILENAME], O_RDWR | O_TRUNC);

    lseek(lfd, 0L, 0);
    lseek(rfd, 0L, 0);
    while ((read_len = read(lfd, buf, BUF_SIZE)) > 0)
        write(ofd, buf, read_len);
    while ((read_len = read(rfd, buf, BUF_SIZE)) > 0)
        write(ofd, buf, read_len);
    
    if (close(ofd) == -1) perror("Error: ");
    if (close(lfd) == -1) perror("Error: ");
    if (close(rfd) == -1) perror("Error: ");
    if (remove("left") == -1) perror("Error: ");
    if (remove("right") == -1) perror("Error: ");

    return 0;
}