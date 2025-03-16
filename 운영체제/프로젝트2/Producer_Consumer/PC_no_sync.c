#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

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
volatile int stop_flag = 0;

int counter = 0;

int main() {
    pthread_t producers[2], consumers[2];
    int producer_ids[2] = {1, 2};
    int consumer_ids[2] = {1, 2};

    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, packet_producer, &producer_ids[i]);
        pthread_create(&consumers[i], NULL, packet_consumer, &consumer_ids[i]);
    }

    usleep(2000000);
    stop_flag = 1;

    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    return 0;
}

void *packet_producer(void *arg) {
    int id = *((int *)arg);
    int packet_id = 0;

    while (!stop_flag) {
        while (counter == BUFFER_SIZE);

        Packet new_packet;
        new_packet.id = packet_id++;
        snprintf(new_packet.data, MAX_PACKET_LEN, "Packet-%d from Producer-%d", new_packet.id, id);

        buffer[in] = new_packet;
        in = (in + 1) % BUFFER_SIZE;
        counter++;

        printf("생산자 : Packet #%d 전송\n", new_packet.id);

        usleep(100000);
    }
    return NULL;
}

void *packet_consumer(void *arg) {
    int id = *((int *)arg);

    while (!stop_flag) {
        while (counter == 0);

        Packet packet = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        counter--;

        printf("----------------------\n");
        printf("소비자 : Packet #%d 수신\n", packet.id);

        usleep(150000);
    }
    return NULL;
}
