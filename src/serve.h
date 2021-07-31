#ifndef SERVE_H
#define SERVE_H

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

#include "dict.h"
#include "request.h"

#define resp_code(status)	respond(server->fds[i].fd, status, "")
#define format_status(msg)	snprintf(strstatus, sizeof(strstatus), \
					"%d %s", \
					status, msg)

enum httpstatus {
	STATUS_OK = 200,
	STATUS_CREATED = 201,
	STATUS_NO_CONTENT = 204,
	STATUS_BAD_REQUEST = 400,
	STATUS_NOT_FOUND = 404,
	STATUS_NOT_IMPLEMENTED = 501,
};

struct httpserver {
	char *port;
	int fd;

	struct pollfd *fds;
	nfds_t nfds;
	nfds_t fdsz;
};

int init_server(struct httpserver *server);
int run_server(struct httpserver *server, struct dict *dict);
void free_server(struct httpserver *server);

int accept_client(struct httpserver *server);
void respond(int clientfd, enum httpstatus status, char *body);

#endif
