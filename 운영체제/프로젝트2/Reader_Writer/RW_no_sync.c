#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_ITEMS 10
#define BUFFER_SIZE 2048

int db[MAX_ITEMS];
int write_index = 0;
int stop_flag = 0;

void *reader(void *arg);
void *writer(void *arg);

int main() {
    pthread_t readers[3], writers[1];
    int reader_ids[3] = {1, 2, 3};
    int writer_ids[1] = {1};

    // 쓰레드 생성
    pthread_create(&writers[0], NULL, writer, &writer_ids[0]);
    for (int i = 0; i < 3; i++) {
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }

    // 실행 후 일정 시간 대기
    usleep(800000);
    stop_flag = 1;

    // 쓰레드 종료 대기
    pthread_join(writers[0], NULL);
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }

    return 0;
}

void *reader(void *arg) {
    int id = *((int *)arg);
    while (!stop_flag) {
        // 데이터 읽기
        char buffer[BUFFER_SIZE] = "";
        char temp[64];
        snprintf(temp, sizeof(temp), "Reader %d : 서버의 데이터 읽기 {", id);
        strncat(buffer, temp, BUFFER_SIZE - strlen(buffer) - 1);

        // 동기화 없이 데이터 출력
        for (int i = 0; i < write_index; i++) {
            snprintf(temp, sizeof(temp), "%d", db[i]);
            strncat(buffer, temp, BUFFER_SIZE - strlen(buffer) - 1);
            if (i != write_index - 1) {
                strncat(buffer, ", ", BUFFER_SIZE - strlen(buffer) - 1);
            }
        }

        strncat(buffer, "}\n", BUFFER_SIZE - strlen(buffer) - 1);
        printf("%s", buffer);

        usleep(id * 50000); // Reader별 속도 차이
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *((int *)arg);
    int fib[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34}; // 피보나치 수열
    for (int i = 0; i < MAX_ITEMS; i++) {
        // 동기화 없이 데이터 쓰기
        db[write_index] = fib[i];
        printf("Writer : 서버 데이터 추가 %d\n", fib[i]);
        printf("------------------------------\n");
        write_index++;

        usleep(50000); // 쓰기 간격
    }
    return NULL;
}
