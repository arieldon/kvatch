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

enum httpstatus {
	STATUS_OK = 200,
	STATUS_CREATED = 201,
	STATUS_NO_CONTENT = 204,
	STATUS_BAD_REQUEST = 400,
	STATUS_NOT_FOUND = 404,
	STATUS_NOT_IMPLEMENTED = 501,
};

int init_server(char *port);
int accept_client(int serverfd, struct pollfd *fds[], nfds_t *nfds, nfds_t *fdsz);
void send_response(int clientfd, enum httpstatus status, char *body);

#endif
