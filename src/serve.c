#include "serve.h"

int
init_server(char *port)
{
	int fd, status, t = 1;
	struct addrinfo *res, *p;
	struct addrinfo hints = { 0 };

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	for (p = res; p != NULL; p = p->ai_next) {
		fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (fd == -1) {
			perror("socket");
			continue;
		}

		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &t,
				sizeof(t)) == -1) {
			close(fd);
			perror("setsockopt");
			continue;
		}

		if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(fd);
			perror("bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "Unable to bind to any socket.\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(res);

	if (listen(fd, 16) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	return fd;
}

static void *
in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int
accept_client(int serverfd)
{
	int clientfd;
	struct sockaddr_storage clientaddr;
	socklen_t clientaddrlen = sizeof(struct sockaddr);
	char clientaddrstr[INET6_ADDRSTRLEN];

	if ((clientfd = accept(serverfd, (struct sockaddr *)&clientaddr,
			&clientaddrlen)) == -1) {
		perror("accept");
		return -1;
	}

	inet_ntop(clientaddr.ss_family, in_addr((struct sockaddr *)&clientaddr),
			clientaddrstr, INET6_ADDRSTRLEN);

	return clientfd;
}

void
send_response(int clientfd, char *msg)
{
	write(clientfd, msg, strlen(msg));
}
