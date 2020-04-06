#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
//필요하면 header file 추가 가능

#define BUF_SIZE 100
#define REC_SIZE 100
#define BUF_PADD 32

enum args {
    SRC_FILENAME = 1,
};

int get_file_size(const char *filename);
void sread(char* source_filename);

int main(int argc, char **argv) {
    // 표준입력으로 받은 레코드 파일에 저장되어 있는 전체 레코드를 "순차적"으로 읽어들이고, 이때
    // 걸리는 시간을 측정하는 코드 구현함
    if (argc != 2) {
        puts("It needs 1 arguments");
        puts("Usage: ./a.out filename"); return 1;
    }
    struct timeval startTime, endTime;
    double diffTime;
    gettimeofday(&startTime, NULL);

    sread(argv[SRC_FILENAME]);

    gettimeofday(&endTime, NULL);
    diffTime = (endTime.tv_sec - startTime.tv_sec) * 1000 * 1000 + (endTime.tv_usec - startTime.tv_usec);
    int num_of_records = get_file_size(argv[SRC_FILENAME]) / REC_SIZE;
    printf("#records: %d timecost: %.0f us", num_of_records, diffTime);

    return 0;
}

int get_file_size(const char *filename) {
    struct stat st;
    stat(filename, &st);
    off_t size = st.st_size;

    return (int)size;
}

void sread(char* source_filename) {
    // sequential read
    char buf[BUF_SIZE + BUF_PADD];
    int src_fd;
    src_fd = open(source_filename, O_RDONLY);
    if (src_fd < 0) { perror("Error"); exit(1); }

    size_t size = 0;
    while ((size = read(src_fd, buf, BUF_SIZE)) > 0) {
        // write(1, buf, size);
    }
    if (close(src_fd) == -1) { perror("Error"); exit(1); }
}
