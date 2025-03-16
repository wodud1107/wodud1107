#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define REQUEST_FIFO_NAME "request_pipe"
#define RESPONSE_FIFO_NAME "response_pipe"
#define BUFFER_SIZE 500

void handle_client_request(char* message, int response_fd){
    char oper, filename[100], data[BUFFER_SIZE];
    int bytes;
    sscanf(message, "%c %s %d %[^\n]", &oper, filename, &bytes, data);

    if(oper=='r'){
        int file_fd=open(filename, O_RDONLY);
        if (file_fd==-1){
            snprintf(data, BUFFER_SIZE, "Unable to open file %s.\n",filename);
            write(response_fd, data, strlen(data)+1);
            perror("File is not opened.\n");
            return;
        }

        int r_bytes=read(file_fd, data, bytes);
        if (r_bytes<0){
            snprintf(data, BUFFER_SIZE, "Unable to read file %s.\n", filename);
        } else{
            data[r_bytes]='\0';
        }
        write(response_fd, data, strlen(data)+1);

        close(file_fd);
    }
    else if (oper=='w'){
        int file_fd=open(filename, O_WRONLY | O_CREAT, 0666);
        if (file_fd==-1){
            snprintf(data, BUFFER_SIZE, "Unable to open file %s.\n", filename);
            write(response_fd, data, strlen(data)+1);
            perror("File is not opened.\n");
            return;
        }

        int w_bytes=write(file_fd, data, bytes);
        snprintf(data, BUFFER_SIZE, "Writing %d bytes to file %s is completed.\n", w_bytes, filename);
        write(response_fd, data, strlen(data)+1);

        close(file_fd);
    }
}

int main(){
    int request_fd, response_fd;
    char buffer[BUFFER_SIZE];

    if(access(REQUEST_FIFO_NAME, F_OK)==-1){
        if(mkfifo(REQUEST_FIFO_NAME, 0666)==-1){
            fprintf(stderr, "request pipe creation is failed.\n");
            exit(1);
        }
        fprintf(stdout, "FIFO '%s' created.\n",REQUEST_FIFO_NAME);
    }else {
        fprintf(stdout, "FIFO '%s' already exists.\n",REQUEST_FIFO_NAME);
    }
    if(access(RESPONSE_FIFO_NAME, F_OK)==-1){
        if(mkfifo(RESPONSE_FIFO_NAME, 0666)==-1){
            fprintf(stderr, "response pipe creation is failed.\n");
            exit(1);
        }
        fprintf(stdout, "FIFO '%s' created.\n",RESPONSE_FIFO_NAME);
    }else {
        fprintf(stdout, "FIFO '%s' already exists.\n",RESPONSE_FIFO_NAME);
    }

    fprintf(stdout, "SERVER started. Waiting for clients.\n");

    while(1){
        request_fd=open(REQUEST_FIFO_NAME, O_RDONLY);
        response_fd=open(RESPONSE_FIFO_NAME, O_WRONLY);
        if (request_fd==-1){
            fprintf(stderr, "Request pipe open error\n");
            exit(1);
        }
        if (response_fd==-1){
            fprintf(stderr, "Response pipe open error\n");
            exit(1);
        }

        if (read(request_fd, buffer, BUFFER_SIZE)>0){
            fprintf(stdout,"Received Request : %s\n", buffer);

            pid_t pid=fork();
            if(pid==0){
                handle_client_request(buffer, response_fd);
                exit(0);
            }
        }

        close(request_fd);
        close(response_fd);
    }

    return 0;
}