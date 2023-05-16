#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX 1024
#define PORT 2222
#define SA struct sockaddr
// 
// Function designed for chat between client and server.
void func(int sockfd) {
    // get input from user and send to server
    char buff[MAX];
    int n = 0;
    bzero(buff, sizeof(buff));

    fd_set rfds;
    fd_set socketread;
    int retval;

    /* Watch stdin to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    /* Watch sockfd to see when it has input. */
    FD_ZERO(&socketread);
    FD_SET(sockfd, &socketread);

    pid_t pid;
    if (pid = fork()) {
        while (1) {
            retval = select(1, &rfds, NULL, NULL, NULL);
            if (retval == -1)
                perror("select()");
            else {
                while ((buff[n++] = getchar()) != '\n');
            }
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(char) * n);

            int wstatus;
            int child_dead = waitpid(pid, &wstatus, WNOHANG);

            if (child_dead) {
                printf("Parent: Server closed connection\n");
                close(sockfd);
                exit(0);
            }
        }
    } else {
        while (1) {
            retval = select(sockfd + 1, &socketread, NULL, NULL, NULL);
            if (retval == -1)
                perror("select()");
            else {
                n = read(sockfd, buff, sizeof(buff));
                printf("From Server : %s", buff);
                bzero(buff, sizeof(buff));

                if (n == 0) {
                    printf("Server closed connection\n");
                    close(sockfd);
                    exit(0);
                }
            }
        }
    }
}

// Driver function
int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect to localhost 2222 for quick chat
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else printf("connected to the server..\n");

    // Function for chatting between client and server
    func(sockfd);
}

