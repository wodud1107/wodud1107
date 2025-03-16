#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h> // O_CREAT, O_EXCL

#define MAX_ITEMS 10
#define BUFFER_SIZE 2048

int db[MAX_ITEMS];
int write_index = 0;
int stop_flag = 0;

sem_t *mutex;
sem_t *rw_mutex;
int read_count = 0;

void *reader(void *arg);
void *writer(void *arg);

int main() {
    pthread_t readers[3], writers[1];
    int reader_ids[3] = {1, 2, 3};
    int writer_ids[1] = {1};

    // 세마포어 초기화
    mutex = sem_open("/mutex", O_CREAT, 0666, 1);
    rw_mutex = sem_open("/rw_mutex", O_CREAT, 0666, 1);

    if (mutex == SEM_FAILED || rw_mutex == SEM_FAILED) {
        perror("sem_open failed");
        return 1;
    }

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

    // 세마포어 해제
    sem_close(mutex);
    sem_close(rw_mutex);
    sem_unlink("/mutex");
    sem_unlink("/rw_mutex");

    return 0;
}

void *reader(void *arg) {
    int id = *((int *)arg);
    while (!stop_flag) {
        sem_wait(mutex); // read_count 보호
        read_count++;
        if (read_count == 1) {
            sem_wait(rw_mutex); // 첫 Reader가 데이터 접근 시 rw_mutex 잠금
        }
        sem_post(mutex);

        // 데이터 읽기
        char buffer[BUFFER_SIZE] = "";
        char temp[64];
        snprintf(temp, sizeof(temp), "Reader %d : 서버의 데이터 읽기 {", id);
        strncat(buffer, temp, BUFFER_SIZE - strlen(buffer) - 1);

        sem_wait(mutex); // 동기화된 상태에서 데이터 출력
        for (int i = 0; i < write_index; i++) {
            snprintf(temp, sizeof(temp), "%d", db[i]);
            strncat(buffer, temp, BUFFER_SIZE - strlen(buffer) - 1);
            if (i != write_index - 1) {
                strncat(buffer, ", ", BUFFER_SIZE - strlen(buffer) - 1);
            }
        }
        sem_post(mutex);

        strncat(buffer, "}\n", BUFFER_SIZE - strlen(buffer) - 1);
        printf("%s", buffer);

        // Reader가 데이터 읽기 완료
        sem_wait(mutex);
        read_count--;
        if (read_count == 0) {
            sem_post(rw_mutex); // 마지막 Reader가 데이터 접근 해제
        }
        sem_post(mutex);

        usleep(id * 50000); // Reader별 속도 차이
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *((int *)arg);
    int fib[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34}; // 피보나치 수열
    for (int i = 0; i < MAX_ITEMS; i++) {
        sem_wait(rw_mutex); // Writer 동기화

        // 데이터 쓰기
        db[write_index++] = fib[i];
        printf("Writer : 서버 데이터 추가 %d\n", fib[i]);
        printf("------------------------------\n");

        sem_post(rw_mutex);
        usleep(50000); // 쓰기 간격
    }
    return NULL;
}
