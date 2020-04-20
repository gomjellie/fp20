#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
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

int dd_write(int ppn, char *pagebuf);
int dd_read(int ppn, char *pagebuf);

void create_flash_memory(const char* flash_file, int block_num);
void write_page(const char* flash_file, int ppn, const char* sector_data, const char* spare_data);
void read_page(const char* flash_file, int ppn);
void erase_block(void);

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
    printf("flash_file : %s, block_num : %d", flash_file, block_num);

    flashfp = fopen(flash_file, "r+");
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

    int res = dd_write(ppn, page_buff);
    if (res == -1) { perror("Error"); exit(1); }
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

void erase_block(void) {
    
}