#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <sys/stat.h>

#define BUFFER_SIZE 5
#define MAX_PACKET_LEN 50

void *packet_producer(void *arg);
void *packet_consumer(void *arg);

typedef struct {
    int id;
    char data[MAX_PACKET_LEN];
} Packet;

Packet buffer[BUFFER_SIZE];
int in = 0, out = 0;
int packet_id = 0;

sem_t *empty, *full;
pthread_mutex_t mutex;

int main() {
    pthread_t producers[2], consumers[2];
    int producer_ids[2] = {1, 2};
    int consumer_ids[2] = {1, 2};

    empty = sem_open("/empty", O_CREAT, S_IRUSR | S_IWUSR, BUFFER_SIZE);
    full = sem_open("/full", O_CREAT, S_IRUSR | S_IWUSR, 0);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, packet_producer, &producer_ids[i]);
        pthread_create(&consumers[i], NULL, packet_consumer, &consumer_ids[i]);
    }

    usleep(2000000);

    for (int i = 0; i < 2; i++) {
        pthread_cancel(producers[i]);
        pthread_cancel(consumers[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    sem_close(empty);
    sem_close(full);
    sem_unlink("/empty");
    sem_unlink("/full");
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *packet_producer(void *arg) {
    int id = *((int *)arg);

    while (1) {
        sem_wait(empty);
        pthread_mutex_lock(&mutex);

        Packet new_packet;
        new_packet.id = packet_id++;
        snprintf(new_packet.data, MAX_PACKET_LEN, "Packet-%d from Producer-%d", new_packet.id, id);

        buffer[in] = new_packet;
        in = (in + 1) % BUFFER_SIZE;

        printf("생산자 : Packet #%d 전송\n", new_packet.id);

        pthread_mutex_unlock(&mutex);
        sem_post(full);

        usleep(100000);
    }
    return NULL;
}

void *packet_consumer(void *arg) {
    int id = *((int *)arg);

    while (1) {
        sem_wait(full);
        pthread_mutex_lock(&mutex);

        Packet packet = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        printf("----------------------\n");
        printf("소비자 : Packet #%d 수신\n", packet.id);

        pthread_mutex_unlock(&mutex);
        sem_post(empty);

        usleep(150000);
    }
    return NULL;
}
