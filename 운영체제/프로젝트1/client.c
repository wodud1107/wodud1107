#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define REQUEST_FIFO_NAME "request_pipe"
#define RESPONSE_FIFO_NAME "response_pipe"
#define BUFFER_SIZE 500

int main() {
    int request_fd, response_fd;
    char oper, filename[100], data[BUFFER_SIZE];
    int bytes;

    request_fd=open(REQUEST_FIFO_NAME, O_WRONLY);
    if (request_fd == -1){
        perror("Request pipe is not opened.\n");
        exit(1);
    }
    response_fd=open(RESPONSE_FIFO_NAME, O_RDONLY);
    if (response_fd==-1){
        perror("Response pipe is not opened.\n");
        close(request_fd);
        exit(1);
    }
    while(1){
        printf("\nEnter the request for the operation\n");
        printf("read for r / write for w / quit for q : ");
        scanf(" %c", &oper);

        if(oper=='q'){
            write(request_fd, "exit", strlen("exit")+1);
            printf("EXIT\n");
            break;
        }

        if (oper=='r'){
            printf("Enter filename: ");
            scanf("%s", filename);
            printf("How many bytes need to read: ");
            scanf("%d", &bytes);

            char message[BUFFER_SIZE];
            snprintf(message, BUFFER_SIZE, "%c %s %d", oper, filename, bytes);
            write(request_fd, message, strlen(message)+1);
            char response[BUFFER_SIZE];
            read(response_fd, response, BUFFER_SIZE);
            printf("Server response: %s\n",response);
        }else if (oper=='w'){
            printf("Enter filename: ");
            scanf("%s", filename);
            printf("Write the data for file.\n");
            printf("===========================\n");
            
            getchar();
            scanf("%[^\n]", data);
            bytes=strlen(data);

            char message[BUFFER_SIZE];
            snprintf(message, BUFFER_SIZE, "%c %s %d %s", oper, filename, bytes, data);
            write(request_fd, message, strlen(message)+1);
            char response[BUFFER_SIZE];
            read(response_fd, response, BUFFER_SIZE);
            printf("Server response: %s\n",response);
        }else{
            printf("Invalid operation.\n");
        }
    }
    

    // FIFO 닫기
    close(request_fd);
    close(response_fd);

    return 0;
}
