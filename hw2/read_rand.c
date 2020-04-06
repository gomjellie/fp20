#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
//필요하면 header file 추가 가능


#define REC_SIZE 100
#define SUFFLE_NUM 10000   // 이 값은 마음대로 수정할 수 있음
#define BUF_SIZE 100
#define BUF_PADD 32

enum args {
    FILENAME = 1,
};

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
int get_file_size(const char* filename);
void rread(const char* filename, const int* order_list, int rec_len);
// 필요한 함수가 있으면 더 추가할 수 있음

//
// argv[1]: 레코드 파일명
//
int main(int argc, char **argv) {
    int num_of_records = get_file_size(argv[FILENAME]) / REC_SIZE;
    int* read_order_list = (int*) malloc(sizeof(int) * num_of_records);
    struct timeval startTime, endTime;
    double diffTime;
    gettimeofday(&startTime, NULL);

    // 아래 함수를 실행하면 'read_order_list' 배열에 추후 랜덤하게 읽어야 할 레코드 번호들이 순서대로 나열되어 저장됨
    // 'num_of_records'는 레코드 파일에 저장되어 있는 전체 레코드의 수를 의미함
    GenRecordSequence(read_order_list, num_of_records);

    rread(argv[FILENAME], read_order_list, num_of_records);
    
    gettimeofday(&endTime, NULL);
    diffTime = (endTime.tv_sec - startTime.tv_sec) * 1000 * 1000 + (endTime.tv_usec - startTime.tv_usec);
    printf("timecost: %.0f us", diffTime);
    
    return 0;
}

void GenRecordSequence(int *list, int n) {
    srand((unsigned int)time(0));

    for(int i = 0; i < n; i++) {
        list[i] = i;
    }
    
    for(int i = 0; i < SUFFLE_NUM; i++) {
        int j = rand() % n;
        int k = rand() % n;
        swap(&list[j], &list[k]);
    }
}

void swap(int *a, int *b) {
    int tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;

    return;
}

int get_file_size(const char *filename) {
    struct stat st;
    stat(filename, &st);
    off_t size = st.st_size;

    return (int)size;
}

void rread(const char* filename, const int* order_list, int rec_len) {
    char buf[BUF_SIZE + BUF_PADD];
    int src_fd;
    src_fd = open(filename, O_RDONLY);
    if (src_fd < 0) { perror("Error"); exit(1); }

    for (int i = 0; i < rec_len; i++) {
        lseek(src_fd, order_list[i] * REC_SIZE, SEEK_SET);
        read(src_fd, buf, REC_SIZE);
        // write(1, buf, REC_SIZE);
    }
    
    if (close(src_fd) == -1) { perror("Error"); exit(1); }
}
