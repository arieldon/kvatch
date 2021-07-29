#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT	"4000"

int init_server(char *port);
int accept_client(int serverfd, struct pollfd *fds[], nfds_t *nfds, nfds_t *fdsz);
void send_response(int clientfd, char *msg);

#endif
