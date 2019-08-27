#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1000    // size of buffer use to read write
#define MAX_QUEUE_BACKLOG 0 // maximum amount of queue
#define MAX_CONNECTION 1    // max amount of user can join group chat
#define PORT 8080           // port number server use to listen
#define GREETING "hello client.... you has been connected"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
static int count_connection = 0;
char global_buff[BUFFER_SIZE];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void error_handler(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

// get index position which not using to store fd
// just call this function when count_connection < MAX_CONNECTION 
   int get_unused_index(int *arr, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (arr[i] == -1)
            return i;
    }
    return -1; // if this happen, mean something wrong!!!
}

void *thread_serve_client(void *params)
{
    // detach this thread from main thread
    // no need to join thread later
    pthread_detach(pthread_self());

    char *buff = (char *)malloc(BUFFER_SIZE);
    memset(buff, 0, BUFFER_SIZE);
    const int fd = *(int *)params;

    // send greeting to client
    write(fd, GREETING, strlen(GREETING));

    // read message from client
    int byte;
    do
    {
        byte = read(fd, buff, BUFFER_SIZE);
        // if byte <= 0 mean connection closed or something error
        if (byte <= 0)
            break;

        // enter critical section
        pthread_mutex_lock(&mutex);
        printf("%s", buff);
        memset(buff, 0, byte);
        pthread_mutex_unlock(&mutex);
        memset(buff, 0, byte);
    } while (byte > 0);

    // free buffer
    free(buff);

    // close file discriptor
    close(fd);
    *(int *)params = -1; // set this fd to -1, not use anymore
    count_connection--;  // reduce number of connection
    return NULL;
}

int main(int argc, char const *argv[])
{
    int ret; // return value, check success or not

    // init socket address structure
    sockaddr_in *address = (sockaddr_in *)malloc(sizeof(sockaddr_in));
    if (address == NULL)
        error_handler("allocate socket fail");
    int addr_len = sizeof(*address);

    // create socket endpoint file discriptor
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0)
        error_handler("create endpoint socket fail");

    // create socket address structure
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_family = AF_INET;
    address->sin_port = htons(PORT);

    // bind fd to socket structure
    ret = bind(server_sockfd, (sockaddr *)address, (socklen_t)addr_len);
    if (ret < 0)
        error_handler("bind socket fail");

    // listen on specific port
    ret = listen(server_sockfd, MAX_QUEUE_BACKLOG);
    if (ret < 0)
        error_handler("listen fail");

    // create list file discriptor for new connection
    // prevent race condition when using thread
    int *list_fd = (int *)malloc(MAX_CONNECTION * sizeof(int));
    for (int i = 0; i < MAX_CONNECTION; i++)
        list_fd[i] = -1; // init file discription list

    while (1)
    {
        // waiting accept new connection
        if (count_connection < MAX_CONNECTION)
        {
            int idx = get_unused_index(list_fd, MAX_CONNECTION);
            if (idx == -1)
                error_handler("something went wrong");

            list_fd[idx] =
                accept(server_sockfd, (sockaddr *)address, &addr_len);
            // after accept new connection, list_fd[idx] must >= 0
            if (list_fd[idx] < 0)
                error_handler("accept new connection fail");
            count_connection++;

            pthread_t worker;
            pthread_create(&worker, NULL, thread_serve_client, &list_fd[idx]);
        }

        /* Use to get client IP (useless when dev environment is localhost)
        char ip_addr[INET_ADDRSTRLEN];
        inet_ntop(address->sin_family, &address->sin_addr.s_addr, ip_addr,
                  (socklen_t)addr_len);
        printf("%s has connected\n", ip_addr);
        */
    }

    // free data
    free(address);
    free(list_fd);

    // close server fd
    close(server_sockfd);
    return 0;
}