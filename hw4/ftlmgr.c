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

FILE *flashfp;
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

queue* new_queue();
void queue_pop(queue* this);
void queue_push(queue* this, int val);
bool queue_empty(queue* this);
int queue_front(queue* this);
int queue_size(queue* this);

int sector_mapping_table[DATAPAGES_PER_DEVICE]; // 디바이스에 있는 섹터 개수 - free_sector 개수
int free_block_position;
queue* psq; // physical_sector_queue

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
void ftl_open()
{
    //
    // address mapping table 초기화
    // free block's pbn 초기화
    // address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
    psq = new_queue();
    for (int i = 0; i < DATAPAGES_PER_DEVICE; i++) {
        sector_mapping_table[i] = -1;
        queue_push(psq, i);
    }

    free_block_position = DATABLKS_PER_DEVICE;

    return;
}

//
// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
//
void ftl_read(int lsn, char *sectorbuf)
{
    // 비어있는 ppn을 가져와서 mapping_table[lsn] = ppn으로 바꿈.
    return;
}

void ftl_write(int lsn, char *sectorbuf)
{
    byte page_buff[PAGE_SIZE];
    byte spare_buff[SPARE_SIZE];
    int ppn = sector_mapping_table[lsn];
    if (ppn != -1) {
        queue_push(psq, ppn); // 반납
    }

    int priority_ppn = queue_front(psq); queue_pop(psq);
    memcpy(page_buff, sectorbuf, SECTOR_SIZE);

    // TODO: spare_buff에 lsn을 넣어야됨.
    memcpy(page_buff + SECTOR_SIZE, spare_buff, SPARE_SIZE);
    dd_write(priority_ppn, page_buff);
    sector_mapping_table[lsn] = priority_ppn;
    
    return;
}

void ftl_print()
{

    return;
}

queue* new_queue() {
    queue* this = (queue*) calloc(QUEUE_BUFF_SIZE + 20, sizeof(queue));
    this->capacity = QUEUE_BUFF_SIZE + 20;
    this->front = 0;
    this->rear = 0;
    
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

int main() {
    flashfp = fopen("flash_file", "wr+");
    ftl_open();
}
