#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
//필요하면 header file 추가 가능


#define REC_SIZE 100
#define SUFFLE_NUM 10000   // 이 값은 마음대로 수정할 수 있음

enum args {
    FILENAME = 1,
};

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
int get_file_size(char *filename);
// 필요한 함수가 있으면 더 추가할 수 있음

//
// argv[1]: 레코드 파일명
//
int main(int argc, char **argv) {
    int num_of_records = get_file_size(argv[FILENAME]) / REC_SIZE;
    int* read_order_list = (int*) malloc(sizeof(int) * num_of_records);

    // 아래 함수를 실행하면 'read_order_list' 배열에 추후 랜덤하게 읽어야 할 레코드 번호들이 순서대로 나열되어 저장됨
    // 'num_of_records'는 레코드 파일에 저장되어 있는 전체 레코드의 수를 의미함
    GenRecordSequence(read_order_list, num_of_records);


    // 'read_order_list'를 이용하여 표준 입력으로 받은 레코드 파일로부터 레코드를 random 하게 읽어들이고,
    // 이때 걸리는 시간을 측정하는 코드 구현함


    return 0;
}

void GenRecordSequence(int *list, int n) {
    srand((unsigned int)time(0));

    for(int i = 0; i < n; i++){
        list[i] = i;
    }
    
    for(int i = 0; i < SUFFLE_NUM; i++) {
        int j = rand() % n;
        int k = rand() % n;
        swap(&list[j], &list[k]);
    }

    return;
}

void swap(int *a, int *b) {
    int tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;

    return;
}

int get_file_size(char *filename) {
    struct stat st;
    stat(filename, &st);
    off_t size = st.st_size;

    return (int)size;
}