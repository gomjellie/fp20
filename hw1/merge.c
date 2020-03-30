#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        puts("It needs 2 arguments");
        puts("Usage: ./a.out filename1 filename2"); return 1;
    }

    int fd1, fd2;
    char c;
    fd1 = open(argv[1], O_RDWR| O_CREAT| O_APPEND, 0644);
    if (fd1 == -1) {
        printf("Can't open file %s\n", argv[1]); return 1;
    }

    fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1) {
        printf("Can't open file %s\n", argv[2]);
    }

    while (read(fd2, &c, 1) > 0)
        write(fd1, &c, 1);

    if (close(fd1) == -1) perror("Error: ");
    if (close(fd2) == -1) perror("Error: ");
    
    return 0;
}