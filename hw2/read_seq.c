#include <stdio.h>
#include <sys/time.h>
#include <unistd.h> // for sleep()
//필요하면 header file 추가 가능

//
// argv[1]: 레코드 파일명
//
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

    
    gettimeofday(&endTime, NULL);
    diffTime = (endTime.tv_sec - startTime.tv_sec) * 1000 * 1000 + (endTime.tv_usec - startTime.tv_usec);
    printf("timecost: %.0f us", diffTime);

    return 0;
}
