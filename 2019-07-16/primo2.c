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
       #include <poll.h>


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

    struct pollfd rdfs;
    struct pollfd socketread;

    rdfs.fd = STDIN_FILENO;
    rdfs.events = POLLIN;

    socketread.fd = sockfd;
    socketread.events = POLLIN;

    struct pollfd fds[2];
    fds[0] = rdfs;
    fds[1] = socketread;

    int retval;
    while (1) {
        retval = poll(fds, 2, -1);

        if (fds[0].revents & POLLIN) {
            while ((buff[n++] = getchar()) != '\n');
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(char) * n);
        } else if (fds[1].revents & POLLIN) {
            int val = read(sockfd, buff, sizeof(buff));
            printf("From Server: %s", buff);
            bzero(buff, sizeof(buff));

            if (val == 0) {
                printf("Server closed connection\n");
                close(sockfd);
                exit(0);
            }
        } else {
            printf("Error unknown poll event\n");
            exit(0);
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

