#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1000

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void error_handler(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    int ret; // return value
    sockaddr_in *serv_addr = (sockaddr_in *)malloc(sizeof(sockaddr_in));
    if (serv_addr == NULL)
        error_handler("allocate socket fail");
    int addr_len = sizeof(*serv_addr);

    // create socket endpoint file discriptor
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd < 0)
        error_handler("create endpoint socket fail");

    // create socket server serv_addr structure
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(SERVER_PORT);
    ret = inet_pton(AF_INET, SERVER_IP, &serv_addr->sin_addr);
    if (ret < 1)
        error_handler("invalid ip format string");

    ret = connect(client_sockfd, (sockaddr *)serv_addr, (socklen_t)addr_len);
    if (ret < 0)
        error_handler("connect to server fail");

    char *buff = (char *)malloc(BUFFER_SIZE);
    memset(buff, 0, BUFFER_SIZE);
    int byte;
    byte = read(client_sockfd, buff, BUFFER_SIZE);

    // print greeting from server
    printf("%s\n", buff);

    // get message from client
    printf("input your message bellow\n");
    do
    {
        memset(buff, 0, byte);
        fgets(buff, BUFFER_SIZE, stdin);
        // send message to server
        byte = write(client_sockfd, buff, strlen(buff));
        if (byte <= 0)
            error_handler("something wrong");
    } while (strcmp(buff, "exit\n") != 0);

    free(buff);
    close(client_sockfd);
    return 0;
}