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

int
accept_client(int serverfd, struct pollfd *fds[], nfds_t *nfds, nfds_t *fdsz)
{
	int clientfd;
	struct sockaddr_storage clientaddr;
	socklen_t clientaddrlen = sizeof(struct sockaddr);

	if ((clientfd = accept(serverfd, (struct sockaddr *)&clientaddr,
			&clientaddrlen)) == -1) {
		perror("accept");
		return -1;
	}

	if (*nfds >= *fdsz) {
		nfds_t fdsz_prime = *fdsz * 2;
		struct pollfd *fds_prime = realloc(*fds, sizeof(struct pollfd) * fdsz_prime);
		if (fds_prime == NULL) {
			perror("realloc");
			return -1;
		}

		*fdsz = fdsz_prime;
		*fds = fds_prime;
	}

	(*fds)[*nfds].fd = clientfd;
	(*fds)[*nfds].events = POLLIN;
	++(*nfds);

	return clientfd;
}

void
send_response(int clientfd, char *msg)
{
	write(clientfd, msg, strlen(msg));
}
