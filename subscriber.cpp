#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "utils.h"
#include "helpers.h"

using namespace std;

int main(int argc, char *argv[]) {
    // sockets
    int sockfd;
    struct sockaddr_in serv_addr;
    // buffer to read data from stdin
    char buffer[BUFLEN];
    // port number
    int port = atoi(argv[3]);
    // struct to monitor multiple fds for different events
    struct pollfd pfds[MAXC];
    int rc;

    if (argc != 4) {
        fprintf(stderr, "\n Usage: %s <ID_CLIENT> <IP_SERVER> <PORT_SERVER>\n", argv[0]);
        return 1;
    }

    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    // get client id
    char id[MAX_ID];
    memset(id, 0, MAX_ID);
    strcpy(id, argv[1]);
    uint32_t size_id = strlen(id);

    // create tcp socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket() failed");

    // deactivate Nagle algorithm
    int nagle = 1;
    rc = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &nagle, sizeof(int));
    DIE(rc < 0, "deactivating nagle failed");

    // set port and IP
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    rc = inet_aton(argv[2], &serv_addr.sin_addr);
    DIE(rc == 0, "inet_aton() failed");

    // send connection request to server
    rc = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    DIE(rc < 0, "connect() failed");

    // read data from stdin
    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;

    // add socket
    pfds[1].fd = sockfd;
    pfds[1].events = POLLIN;

    // send message to server (client id)
    rc = send(sockfd, &size_id, sizeof(uint32_t), 0);
    DIE(rc < 0, "send() failed");
    rc = send(sockfd, id, size_id, 0);
    DIE(rc < 0, "send() failed");

    while (1) {
        // wait for readiness notification
        rc = poll(pfds, 2, -1);
        DIE(rc < 0, "poll() failed");

        if ((pfds[0].revents & POLLIN) != 0) {
            // input from stdin
            memset(buffer, 0, BUFLEN);
			cin.getline(buffer, BUFLEN);

            // parse input
            char msg_type[TYPE_LEN];
            struct message_t msg;
            int num = sscanf(buffer, "%s %s %d", msg_type, msg.topic, &msg.sf);

			if (strncmp(msg_type, "exit", 4) == 0) {
                if (num != 1) {
                    fprintf(stderr, "Usage: exit\n");
                    break;
                }
                break;
            } else if (strncmp(msg_type, "subscribe", 9) == 0) {
                if (num != 3) {
                    fprintf(stderr, "Usage: subscribe <TOPIC> <SF>\n");
                    break;
                }

                if (msg.sf != 0 && msg.sf != 1) {
                    fprintf(stderr, "SF must be 0 or 1\n");
                    break;
                }

                msg.type = SUBSCRIBE;
                cout << "Subscribed to topic.\n";
            } else if (strncmp(msg_type, "unsubscribe", 11) == 0) {
                if (num != 2) {
                    fprintf(stderr, "Usage: unsubscribe <TOPIC>\n");
                    break;
                }

                msg.type = UNSUBSCRIBE;
                cout << "Unsubscribed from topic.\n";
            } else {
                fprintf(stderr, "Unknown command\n");
                break;
            }
            // send message to server
            uint32_t size = sizeof(msg);
            rc = send(sockfd, &size, sizeof(uint32_t), 0);
            if (rc < 0, "send() failed");
            rc = send(sockfd, &msg, size, 0);
            if (rc < 0, "send() failed");

        } else if ((pfds[1].revents & POLLIN) != 0) {
            // receive message from server
            uint32_t size;
            rc = recv(sockfd, &size, sizeof(uint32_t), 0);
            DIE(rc < 0, "recv() failed");

            memset(buffer, 0, BUFLEN);
            rc = recv(sockfd, buffer, size, 0);
            DIE(rc < 0, "recv() failed");

            if (strncmp(buffer, "exit", 4) == 0) {
                break;
            } else {
                // print message
                cout << buffer;
                cout << "\n";
            }
        }
    }     

    close(sockfd);
    return 0;
}