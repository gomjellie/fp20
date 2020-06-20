#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "person.h"

//필요한 경우 헤더 파일과 함수를 추가할 수 있음

const char delimeter = '#';
const size_t REC_PER_PAGE = PAGE_SIZE / RECORD_SIZE;

enum person_len {
    SN    = 14,     //주민번호
    NAME  = 18,     //이름
    AGE   = 4,      //나이
    ADDR  = 22,     //주소
    PHONE = 16,     //전화번호
    EMAIL = 26,     //이메일 주소
};


static void person_print(const Person* this) {
    if (this->sn[0] == '*') { puts("*deleted"); return; }
    printf("sn: %s\n", this->sn);
    printf("name: %s\n", this->name);
    printf("age: %s\n", this->age);
    printf("addr: %s\n", this->addr);
    printf("phone: %s\n", this->phone);
    printf("email: %s\n", this->email);
}

void pack(char *recordbuf, const Person *p) {
    char* rb = recordbuf;
    size_t s;
    memcpy(rb, p->sn    , s = strlen(p->sn));    rb[s] = delimeter; rb += SN;
    memcpy(rb, p->name  , s = strlen(p->name));  rb[s] = delimeter; rb += NAME;
    memcpy(rb, p->age   , s = strlen(p->age));   rb[s] = delimeter; rb += AGE;
    memcpy(rb, p->addr  , s = strlen(p->addr));  rb[s] = delimeter; rb += ADDR;
    memcpy(rb, p->phone , s = strlen(p->phone)); rb[s] = delimeter; rb += PHONE;
    memcpy(rb, p->email , s = strlen(p->email)); rb[s] = delimeter; rb += EMAIL;
}

void unpack(const char *recordbuf, Person *p) {
    int piv = 0;
    int cnt = 0;
    char* entries[] = {
        p->sn, p->name, p->age, p->addr, p->phone, p->email,
    };
    size_t lens[] = {SN, NAME, AGE, ADDR, PHONE, EMAIL};
    for (int i = 0; i < RECORD_SIZE; i++) {
        if (recordbuf[i] == delimeter) {
            memcpy(entries[cnt], recordbuf + piv, i - piv);
            entries[cnt][i - piv] = '\0';
            piv += lens[cnt++];
            i = piv ;
            continue;
        }
    }
}

typedef struct heap_s {
    size_t length;
    size_t capacity;
    Person *buff;
} heap_t;

heap_t *new_heap(size_t capacity) {
    heap_t *this = malloc(sizeof(heap_t));
    this->length = 0;
    this->capacity = capacity;
    this->buff = malloc(sizeof(Person) * capacity);

    return this;
}

void del_heap(heap_t *this) {
    free(this->buff);
    free(this);
}

bool heap_empty(heap_t *this) {
    return this->length == 0;
}

Person *heap_top(heap_t *this) {
    if (heap_empty(this)) return NULL;

    return this->buff + 1;
}

void heap_push(heap_t *this, const Person *element) {
    size_t cur = ++this->length;
    while (cur != 1 && strcmp(element->sn, this->buff[cur / 2].sn) < 0) {
        this->buff[cur] = this->buff[cur / 2];
        cur /= 2;
    }
    this->buff[cur] = *element;
}

void heap_pop(heap_t *this) {
    Person last_element = this->buff[this->length--];
    
    size_t cur = 1;
    size_t child = 2;
    
    while (child <= this->length) {
        if (child < this->length && strcmp(this->buff[child + 1].sn, this->buff[child].sn) < 0)
            child++;
        if (strcmp(last_element.sn, this->buff[child].sn) < 0) break;

        this->buff[cur] = this->buff[child];
        cur = child; child *= 2;
    }
    this->buff[cur] = last_element;
}

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓸 때나
// 모든 I/O는 위의 두 함수를 먼저 호출해야 합니다. 즉, 페이지 단위로 읽거나 써야 합니다.

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum) {
    fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
    fread((void *)pagebuf, PAGE_SIZE, 1, fp);
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 위치에 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum) {
    fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
    fwrite((const void *)pagebuf, PAGE_SIZE, 1, fp);
}

//
// 주어진 레코드 파일에서 레코드를 읽어 heap을 만들어 나간다. Heap은 배열을 이용하여 저장되며, 
// heap의 생성은 Chap9에서 제시한 알고리즘을 따른다. 레코드를 읽을 때 페이지 단위를 사용한다는 것에 주의해야 한다.
//
void buildHeap(FILE *inputfp, char **heaparray) {
    char *meta_page = malloc(PAGE_SIZE);
    char *tar_page  = malloc(PAGE_SIZE);
    readPage(inputfp, meta_page, 0);
    const int np = *((int *)meta_page);        // 전체 페이지 수
    const int nr = *((int *)(meta_page + 4));  // 모든 레코드 수 (삭제 포함)

    heap_t **heap = (heap_t **)heaparray;
    *heap = new_heap(np * REC_PER_PAGE);

    for (int ip = 1; ip < np; ip++) { // iterate page
        readPage(inputfp, tar_page, ip);
        for (int ir = 0; ir < REC_PER_PAGE; ir++) { // iterate record in page
            Person p;
            unpack(tar_page + ir * RECORD_SIZE, &p);
            if (p.sn[0] == '*') continue; // 삭제된 레코드
            
            printf("ip: %d, ir: %d \n", ip, ir);
            heap_push(*heap, &p);
            p.sn[0] = '*';
        }
    }

    while (!heap_empty(*heap)) {
        Person *res = heap_top(*heap);
        printf("sn: %s, name: %s\n", res->sn, res->name);
        heap_pop(*heap);
    }

    free(meta_page);
    free(tar_page);
}

//
// 완성한 heap을 이용하여 주민번호를 기준으로 오름차순으로 레코드를 정렬하여 새로운 레코드 파일에 저장한다.
// Heap을 이용한 정렬은 Chap9에서 제시한 알고리즘을 이용한다.
// 레코드를 순서대로 저장할 때도 페이지 단위를 사용한다.
//
void makeSortedFile(FILE *outputfp, char **heaparray) {
    
}

static void init_flash(FILE** fp, const char* file_name) {
    *fp = fopen(file_name, "w+");
    
    char mem_buff[PAGE_SIZE];
    memset((void *)mem_buff, 0xFF, PAGE_SIZE);
    *((int *)mem_buff)       = 2;       // 전체 페이지 수
    *((int *)(mem_buff + 4)) = 0;       // 모든 레코드 수
    *((int *)(mem_buff + 8)) = -1;      // 삭제된 페이지 번호
    *((int *)(mem_buff + 12)) = -1;     // 삭제된 레코드 번호(페이지 내에서의 번호)
    int ret = fwrite((void *)mem_buff, PAGE_SIZE * 2, 1, *fp);
    if (ret == -1) { perror("Error"); exit(1); }
}

static void show(FILE* fp) {
    char* meta_page = malloc(PAGE_SIZE);
    char* tar_page  = malloc(PAGE_SIZE);
    readPage(fp, meta_page, 0);
    const int np = *((int *)meta_page);        // 전체 페이지 수
    const int nr = *((int *)(meta_page + 4));  // 모든 레코드 수 (삭제 포함)
    const int dp = *((int *)(meta_page + 8));  // 삭제된 페이지 번호
    const int dr = *((int *)(meta_page + 12)); // 삭제된 레코드 번호(페이지 내에서의 번호)
    printf("----메타 데이터-----\n");
    printf("페이지수: %d, 레코드수: %d, 삭제된 레코드: %d페이지의 %d번째\n", np, nr, dp, dr);
    puts("--------");

    for (int ip = 1; ip < np; ip++) { // iterate page
        readPage(fp, tar_page, ip);
        for (int ir = 0; ir < REC_PER_PAGE; ir++) { // iterate record in page
            printf("-------%d 페이지 %d 번째 사람-----\n", ip, ir);
            Person* p = calloc(1, sizeof(Person));
            unpack(tar_page + ir * RECORD_SIZE, p);
            person_print(p);
            free(p);
        }
    }
    free(tar_page);
    free(meta_page);
}

int main(int argc, char *argv[]) {
    FILE *inputfp;    // 입력 레코드 파일의 파일 포인터
    FILE *outputfp;    // 정렬된 레코드 파일의 파일 포인터

    enum args {
        OPTION            = 1,
        INPUT_FILE_NAME   = 2,
        OUTPUT_FILE_NAME  = 3,
    };
    char option = argv[OPTION][0];
    inputfp = fopen(argv[INPUT_FILE_NAME], "r+");
    if (inputfp == NULL) init_flash(&inputfp, argv[INPUT_FILE_NAME]);
    outputfp = fopen(argv[OUTPUT_FILE_NAME], "r+");
    if (outputfp == NULL) init_flash(&outputfp, argv[OUTPUT_FILE_NAME]);

    switch (option) {
        case 's':
            if (argc != 4) {
                puts("not enough arguments!");
                puts("Usage: ./a.out s input_file_name output_file_name");
                exit(1);
            }
            heap_t *heap;
            buildHeap(inputfp, (char **)&heap);
            makeSortedFile(outputfp, (char **)&heap);
            break;
        case 'S':
            show(inputfp);
            break;
        default:
            puts("fall back to default");
    }
    return 0;
}

void test_heap() {
    heap_t *heap = new_heap(16);
    Person jaq = {
        .sn = "8811032129018",
        .name = "Jaq",
    };
    Person jacky = {
        .sn = "9605061234123",
        .name = "Jacky",
    };
    Person mike = {
        .sn = "9712121020304",
        .name = "Mike",
    };
    Person nancy = {
        .sn = "8712341235123",
        .name = "Nancy",
    };
    Person bob = {
        .sn = "9211231234123",
        .name = "Bob",
    };

    Person persons[] = {
        jaq, jacky, mike, nancy, bob,
    };

    for (int i = 0; i < 5; i++) {
        heap_push(heap, &persons[i]);
    }

    while (!heap_empty(heap)) {
        Person *res = heap_top(heap);
        printf("sn: %s, name: %s\n", res->sn, res->name);
        heap_pop(heap);
    }
}
