// 주의사항
// 1. sectormap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. sectormap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(sectormap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "sectormap.h"
// 필요한 경우 헤더 파일을 추가하시오.

extern int dd_read(int ppn, char *pagebuf);
extern int dd_write(int ppn, char *pagebuf);
extern int dd_erase(int pbn);

typedef char byte;

typedef enum _bool {
    false,
    true,
} bool;

#define QUEUE_BUFF_SIZE DATAPAGES_PER_DEVICE + 40

typedef struct _queue {
    int buff[QUEUE_BUFF_SIZE]; // [60 + 40]

    int front;
    int rear;
    int capacity;
} queue;

queue new_queue();
void queue_pop(queue* this);
void queue_push(queue* this, int val);
bool queue_empty(queue* this);
int queue_front(queue* this);
int queue_size(queue* this);

int sector_mapping_table[DATAPAGES_PER_DEVICE]; // 디바이스에 있는 섹터 개수 - free_sector 개수
queue psq; // physical_sector_queue

void ftl_open() {
    // flash memory를 처음 사용할 때 필요한 초기화 작업.
    // ftl_write(), ftl_read() 이전에 호출되어야한다.
    psq = new_queue();
    for (int i = 0; i < DATAPAGES_PER_DEVICE; i++) {
        sector_mapping_table[i] = -1;
        queue_push(&psq, i);
    }
}

void ftl_read(int lsn, char *sectorbuf) {
    // sectorbuf는 호출이전에 이미 512B 메모리가 할당이 되어있다.
    byte page_buff[PAGE_SIZE];
    int ppn = sector_mapping_table[lsn];
    dd_read(ppn, page_buff);
    memcpy(sectorbuf, page_buff, SECTOR_SIZE);
    return;
}

void ftl_write(int lsn, char *sectorbuf) {
    byte page_buff[PAGE_SIZE];
    byte spare_buff[SPARE_SIZE];
    int ppn = sector_mapping_table[lsn];
    if (ppn != -1)
        queue_push(&psq, ppn); // 반납

    int priority_ppn = queue_front(&psq); queue_pop(&psq);
    memcpy(page_buff, sectorbuf, SECTOR_SIZE);
    *((int *)spare_buff) = lsn; // spare_buff에 lsn을 넣음.
    memcpy(page_buff + SECTOR_SIZE, spare_buff, SPARE_SIZE);
    dd_write(priority_ppn, page_buff);
    sector_mapping_table[lsn] = priority_ppn;
    
    return;
}

void ftl_print()
{
    printf("lpn pp \n");
    for (int i = 0 ; i < DATAPAGES_PER_DEVICE; i++) printf("%d %d \n", i, sector_mapping_table[i]);
    printf("free block’s pbn=%d", queue_front(&psq));
    return;
}

queue new_queue() {
    // 소멸자 호출을 넣을수 있는 ftl_close가 없어서 queue 를 동적 메모리로 생성하지 않음.
    queue this = {
        .capacity = QUEUE_BUFF_SIZE,
        .front = 0,
        .rear = 0,
    };
    
    return this;
}

void queue_pop(queue* this) {
    this->front = (this->capacity + this->front + 1) % this->capacity; 
}

void queue_push(queue* this, int val) {
    this->rear = (this->capacity + this->rear + 1) % this->capacity;
    this->buff[this->rear] = val;
}

int queue_size(queue* this) {
    return (this->capacity + this->front - this->rear) % this->capacity;
}

int queue_front(queue* this) {
    return this->buff[this->front + 1];
}

void queue_show(queue* this) {
    while(!queue_empty(this)) {
        printf("%d\n", queue_front(this));
        queue_pop(this);
    }
}

bool queue_empty(queue* this) {
    return this->front == this->rear;
}
