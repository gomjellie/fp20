#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "sectormap.h"

extern void ftl_open();
extern void ftl_read(int lsn, char *sectorbuf);
extern void ftl_write(int lsn, char *sectorbuf);
extern void ftl_print();

FILE *flashfp;

void create_flash_memory(int block_num);

int main() {
    char sector_buf[60][SECTOR_SIZE];
    char free_buf[SECTOR_SIZE];
    char messages[60][30] = {
        "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
        "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen",
        "twenty", "twenty one", "twenty two", "twenty three", "twenty four", "twenty five", "twenty six", "twenty seven", "twenty eight", "twenty nine",
        "thirty", "thirty one", "thirty two", "thirty three", "thirty four", "thirty five", "thirty six", "thirty seven", "thirty eight", "thirty nine",
        "fourty", "fourty one", "fourty two", "fourty three", "fourty four", "fourty five", "fourty six", "fourty seven", "fourty eight", "fourty nine",
        "fifty", "fifty one", "fifty two", "fifty three", "fifty four", "fifty five", "fifty six", "fifty seven", "fifty eight", "fifty nine",
    };
    for (int i = 0; i < 60; i++) {
        memcpy(sector_buf[i], messages[i], strlen(messages[i]) + 1);
    }
    flashfp = fopen("flash_file", "r+");
    create_flash_memory(BLOCKS_PER_DEVICE);
    ftl_open();
    ftl_write(2, sector_buf[2]);
    ftl_write(2, sector_buf[2]);
    ftl_write(2, sector_buf[2]);
    ftl_write(2, sector_buf[2]);

    for (int i = 0; i < 57; i++) {
        ftl_write(i, sector_buf[i]);
    }
    ftl_write(2, sector_buf[2]);
    ftl_write(2, sector_buf[2]);
    ftl_write(2, sector_buf[2]);
    ftl_write(57, sector_buf[57]);
    ftl_print();
    for (int i = 0; i < 57; i++) {
        ftl_read(i, free_buf);
        printf("%d: %s\n", i, free_buf);
    }
}


void create_flash_memory(int block_num) {
    /**
    flash memory 파일 생성: 위에서 선언한 flashfp를 사용하여 flash 파일을 생성한다. 그 이유는 fdevicedriver.c에서 
    flashfp 파일포인터를 extern으로 선언하여 사용하기 때문이다.
    */
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
