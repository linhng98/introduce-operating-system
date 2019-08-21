#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_LENGTH_BUFFER 1000
#define MAX_CONNECTION 2
#define PORT 8080

void error_handler(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    int ret; // return value, check success or not

    /* init socket address structure */
    struct sockaddr_in *address =
        (struct sockadr_in *)malloc(sizeof(struct sockaddr_in));
    if (address == NULL)
        error_handler("allocate fail");

    /* create socket endpoint file discriptor */
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0)
        error_handler("create endpoint socket fail");

    /* create socket address structure */
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_family = AF_INET;
    address->sin_port = htons(PORT);
    memset(address->sin_zero, 0,
           sizeof(address->sin_zero)); // just for padding purpose, no usecase

    /* bind fd to socket structure */
    ret = bind(server_sockfd, address, sizeof(*address));
    if (ret < 0)
        error_handler("bind socket fail");

    /* listen on specific port */
    ret = listen(server_sockfd, MAX_CONNECTION);
    if (ret < 0)
        error_handler("listen fail");

    /* create list file discriptor for new connection */
    int *list_fd = (int *)malloc(MAX_CONNECTION * sizeof(int));
    memset(list_fd, 0, MAX_CONNECTION);
    int count_connection = 0;

    while (1)
    {
        /* waiting accept new connection */
        printf("(%d) waiting for new connection .....", count_connection);
        list_fd[count_connection] =
            accept(server_sockfd, address, sizeof(*address));
        if (list_fd[count_connection] < 0)
            error_handler("accept new connection fail");
        count_connection++;

        char ip_addr[INET_ADDRSTRLEN];
        printf(inet_ntop(address->sin_family, address->sin_addr.s_addr, ))
    }

    /* free data */
    free(address);
    return 0;
}