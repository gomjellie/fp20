#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "flash.h"
// 필요한 경우 헤더파일을 추가한다

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

FILE *flashfp;    // fdevicedriver.c에서 사용

//
// 이 함수는 FTL의 역할 중 일부분을 수행하는데 물리적인 저장장치 flash memory에 Flash device driver를 이용하여 데이터를
// 읽고 쓰거나 블록을 소거하는 일을 한다 (동영상 강의를 참조).
// flash memory에 데이터를 읽고 쓰거나 소거하기 위해서 fdevicedriver.c에서 제공하는 인터페이스를
// 호출하면 된다. 이때 해당되는 인터페이스를 호출할 때 연산의 단위를 정확히 사용해야 한다.
// 읽기와 쓰기는 페이지 단위이며 소거는 블록 단위이다.
// 

enum args {
    OPTION = 1,
    FLASHFILE = 2,
    PPN = 3,
    PBN = 3,
    BLOCK_NUM = 3,
    SECTOR_DATA = 4,
    SPARE_DATA = 5,
};

enum bool {
    false = 0,
    true = 1,
};

int dd_write(int ppn, char *pagebuf);
int dd_read(int ppn, char *pagebuf);
int dd_erase(int pbn);

void create_flash_memory(const char* flash_file, int block_num);
void write_page(const char* flash_file, int ppn, const char* sector_data, const char* spare_data);
void read_page(const char* flash_file, int ppn);
void erase_block(const char* flash_file, int pbn);
enum bool is_empty_page(int ppn);
enum bool is_empty_block(int pbn);
int find_empty_block(int exception_block);
long get_file_size();

int main(int argc, char *argv[])
{    
    char sectorbuf[SECTOR_SIZE];
    char pagebuf[PAGE_SIZE];
    char *blockbuf;
    
    char option = argv[OPTION][0];
    
    switch (option) {
        case 'c':
            create_flash_memory(argv[FLASHFILE], atoi(argv[BLOCK_NUM])); break;
        case 'w':
            write_page(argv[FLASHFILE], atoi(argv[PPN]), argv[SECTOR_DATA], argv[SPARE_DATA]); break;
        case 'r':
            read_page(argv[FLASHFILE], atoi(argv[PPN])); break;
        case 'e':
            erase_block(argv[FLASHFILE], atoi(argv[PBN])); break;
        default:
            printf("invalid option %c", option); break;
    }

    return 0;
}

void create_flash_memory(const char * flash_file, int block_num) {
    /**
    flash memory 파일 생성: 위에서 선언한 flashfp를 사용하여 flash 파일을 생성한다. 그 이유는 fdevicedriver.c에서 
    flashfp 파일포인터를 extern으로 선언하여 사용하기 때문이다.
    */
    flashfp = fopen(flash_file, "w+");
    if (flashfp == NULL) {
        perror("Error"); exit(1);
    }

    char mem_buff[BLOCK_SIZE];
    memset((void *)mem_buff, 0xFF, BLOCK_SIZE);
    
    for (int i = 0; i < block_num; i++) {
        int ret = fwrite((void *)mem_buff, BLOCK_SIZE, 1, flashfp);
        if (ret == -1) { perror("Error"); exit(1); }
    }
}

void write_page(const char* flash_file, int ppn, const char* sector_data, const char* spare_data) {
    /**
    페이지 쓰기: pagebuf의 섹터와 스페어에 각각 입력된 데이터를 정확히 저장하고 난 후 해당 인터페이스를 호출한다
    */
    char page_buff[PAGE_SIZE];
    memset((void *)page_buff, 0xFF, PAGE_SIZE);
    
    flashfp = fopen(flash_file, "r+");

    memcpy(page_buff, sector_data, MIN(strlen(sector_data), SECTOR_SIZE));
    memcpy(page_buff + SECTOR_SIZE, spare_data, MIN(strlen(spare_data), SPARE_SIZE));

    if (is_empty_page(ppn)) {
        int res = dd_write(ppn, page_buff);
        if (res == -1) { perror("Error"); exit(1); }
        
        return;
    }

    // ssd cannot overwrite
    // implementation of in-place update
    int pbn = ppn / PAGE_NUM;
    int empty_block = find_empty_block(pbn);

    if (empty_block == -1) { perror("Error"); exit(1); }

    printf("empty block : %d \n", empty_block);

    char _page_buff[PAGE_SIZE];

    for (int i = 0; i < PAGE_NUM; i++) {
        int tar_page_number = pbn * PAGE_NUM + i;
        int empty_page_number = empty_block * PAGE_NUM + i;
        if (tar_page_number == ppn) {
            // memcpy(block_buff + i * PAGE_SIZE, page_buff, PAGE_SIZE);
            dd_write(empty_page_number, page_buff);
            continue;
        }
        dd_read(tar_page_number, _page_buff); // read from block where it was supposed to be over-written
        dd_write(empty_page_number, _page_buff); // write at empty block
    }
    dd_erase(pbn);

    for (int i = 0; i < PAGE_NUM; i++) {
        int tar_page_number = pbn * PAGE_NUM + i;
        int empty_page_number = empty_block * PAGE_NUM + i;

        dd_read(empty_page_number, _page_buff);
        dd_write(tar_page_number, _page_buff);
    }
    dd_erase(empty_block);
}

void read_page(const char* flash_file, int ppn) {
    /**
    페이지 읽기: pagebuf를 인자로 사용하여 해당 인터페이스를 호출하여 페이지를 읽어 온 후 여기서 섹터 데이터와
    스페어 데이터를 분리해 낸다
    memset(), memcpy() 등의 함수를 이용하면 편리하다. 물론, 다른 방법으로 해결해도 무방하다.
    */
    flashfp = fopen(flash_file, "r+");
    
    char page_buff[PAGE_SIZE];
    char sector_buff[SECTOR_SIZE];
    char spare_buff[SPARE_SIZE];
    
    int res = dd_read(ppn, page_buff);
    if (res == -1) { perror("Error"); exit(1); }
    
    memcpy(sector_buff, page_buff, SECTOR_SIZE);
    memcpy(spare_buff, page_buff + SECTOR_SIZE, SPARE_SIZE);

    if (sector_buff[0] == (char)0xff && spare_buff[0] == (char)0xff) return;
    for (int i = 0; i < SECTOR_SIZE; i++) {
        if (sector_buff[i] == (char)0xff) break;

        putchar(sector_buff[i]);
    }
    putchar(' ');
    for (int i = 0; i < SPARE_SIZE; i++) {
        if (spare_buff[i] == (char)0xff) break;

        putchar(sector_buff[i]);
    }
}

void erase_block(const char* flash_file, int pbn) {
    flashfp = fopen(flash_file, "r+");

    dd_erase(pbn);
}

enum bool is_empty_page(int ppn) { // physical page number
    char page_buff[PAGE_SIZE];
    
    dd_read(ppn, page_buff);
    if (page_buff[0] == (char)0xff && page_buff[SECTOR_SIZE] == (char)0xff)
        return true;
    return false;
}

enum bool is_empty_block(int pbn) { // physical block number
    int base_ppn = pbn * PAGE_NUM;
    for (int ppn = base_ppn; ppn < base_ppn + PAGE_NUM; ppn++) {
        if (!is_empty_page(ppn)) return false;
    }
    return true;
}

int find_empty_block(int exception_block) {
    int top_block = get_file_size() / BLOCK_SIZE;

    for (int pbn = 0; pbn < top_block; pbn++) {
        if (pbn == exception_block) continue;
        if (is_empty_block(pbn)) return pbn;
    }

    return -1;
}

long get_file_size() {
    fseek(flashfp, 0, SEEK_END);    // 파일 포인터를 파일의 끝으로 이동시킴
    long size = ftell(flashfp);

    return size;
}
