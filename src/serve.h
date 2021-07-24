#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT	"4000"

int init_server(char *port);
int accept_client(int serverfd);
void send_response(int clientfd, char *msg);

#endif
