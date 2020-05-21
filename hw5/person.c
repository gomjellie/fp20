#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

enum init {
    PAGE_LEN = 10000,
};

enum args {
    OPTION = 1,
    FILE_NAME = 2,
    FIELD_SN = 3,
    FIELD_NAME = 4,
    FIELD_AGE = 5,
    FIELD_ADDR = 6,
    FIELD_PHONE = 7,
    FIELD_EMAIL = 8,
};

const char delimeter = '#';

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 모든 I/O는 위의 두 함수를 먼저 호출해야 합니다. 즉 페이지 단위로 읽거나 써야 합니다.

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

enum person_len {
    SN = 14,        //주민번호
    NAME = 18,      //이름
    AGE = 4,        //나이
    ADDR = 22,      //주소
    PHONE = 16,     //전화번호
    EMAIL = 26,     //이메일 주소
};
//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 그런 후 이 레코드를 저장할 페이지를 readPage()를 통해 프로그램 상에
// 읽어 온 후 pagebuf에 recordbuf에 저장되어 있는 레코드를 저장한다. 그 다음 writePage() 호출하여 pagebuf를 해당 페이지 번호에
// 저장한다. pack() 함수에서 readPage()와 writePage()를 호출하는 것이 아니라 pack()을 호출하는 측에서 pack() 함수 호출 후
// readPage()와 writePage()를 차례로 호출하여 레코드 쓰기를 완성한다는 의미이다.
// 
void pack(char *recordbuf, const Person *p) {
    char* rb = recordbuf;
    size_t s;
    strncpy(rb, p->sn    , s = strlen(p->sn));    rb[s] = delimeter; rb += SN;
    strncpy(rb, p->name  , s = strlen(p->name));  rb[s] = delimeter; rb += NAME;
    strncpy(rb, p->age   , s = strlen(p->age));   rb[s] = delimeter; rb += AGE;
    strncpy(rb, p->addr  , s = strlen(p->addr));  rb[s] = delimeter; rb += ADDR;
    strncpy(rb, p->phone , s = strlen(p->phone)); rb[s] = delimeter; rb += PHONE;
    strncpy(rb, p->email , s = strlen(p->email)); rb[s] = delimeter; rb += EMAIL;
}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다. 이 함수가 언제 호출되는지는
// 위에서 설명한 pack()의 시나리오를 참조하면 된다.
//

void unpack(const char *recordbuf, Person *p) {
    int piv = 0;
    int cnt = 0;
    char* entries[] = {
        p->sn, p->name, p->age, p->addr, p->phone, p->email,
    };
    size_t lens[] = {SN, NAME, AGE, ADDR, PHONE, EMAIL};
    for (int i = 0; i < RECORD_SIZE; i++) {
        if (recordbuf[i] == delimeter) {
            strncpy(entries[cnt], recordbuf + piv, i - piv);
            entries[cnt][i - piv] = '\0';
            piv += lens[cnt++];
            i = piv ;
            continue;
        }
    }
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값을 구조체에 저장한 후 아래의 insert() 함수를 호출한다.
//
void insert(FILE *fp, const Person *p) {

}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *sn) {

}

void init_flash(FILE** fp, const char* file_name) {
    *fp = fopen(file_name, "w+");
    
    char mem_buff[PAGE_SIZE];
    memset((void *)mem_buff, 0xFF, PAGE_SIZE);
    
    for (int i = 0; i < PAGE_LEN; i++) {
        int ret = fwrite((void *)mem_buff, PAGE_SIZE, 1, *fp);
        if (ret == -1) { perror("Error"); exit(1); }
    }
}

Person* new_person(const char* sn, const char* name, const char* age, const char* addr, const char* phone, const char* email) {
    Person* this = (Person *)malloc(sizeof(Person));
    strncpy(this->sn, sn, SN);
    strncpy(this->name, name, NAME);
    strncpy(this->age, age, AGE);
    strncpy(this->addr, addr, ADDR);
    strncpy(this->phone, phone, PHONE);
    strncpy(this->email, email, EMAIL);
    return this;
}

void person_print(Person* this) {
    printf("sn: %s\n", this->sn);
    printf("name: %s\n", this->name);
    printf("age: %s\n", this->age);
    printf("addr: %s\n", this->addr);
    printf("phone: %s\n", this->phone);
    printf("email: %s\n", this->email);
}

int main(int argc, char *argv[]) {
    FILE *fp;  // 레코드 파일의 파일 포인터
    
    char option = argv[OPTION][0];
    
    switch (option) {
        case 'i':
            fp = fopen(argv[FILE_NAME], "r+");
            if (fp == NULL) {
                init_flash(&fp, argv[FILE_NAME]);
            }
            Person* p = new_person(argv[FIELD_SN], argv[FIELD_NAME], argv[FIELD_AGE], argv[FIELD_ADDR], argv[FIELD_PHONE], argv[FIELD_EMAIL]);
            // person_print(p);
            char* buff = (char *)calloc(RECORD_SIZE, sizeof(char));
            pack(buff, p);
            Person res;
            unpack(buff, &res);
            person_print(&res);
            break;
        case 'd':
            fp = fopen(argv[FILE_NAME], "r+");
            break;
        default:
            printf("invalid option %c", option); break;
    }
    return 0;
}
