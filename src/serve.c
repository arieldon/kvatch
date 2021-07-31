#include "serve.h"

int
init_server(struct httpserver *server)
{
	int status, t = 1;
	struct addrinfo *res, *p;
	struct addrinfo hints = { 0 };

	server->port = "4000";

	server->nfds = 0;
	server->fdsz = 8;
	server->fds = calloc(server->fdsz, sizeof(struct pollfd));
	if (server->fds == NULL) {
		perror("calloc");
		return -1;
	}

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	if ((status = getaddrinfo(NULL, server->port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
		return -1;
	}

	for (p = res; p != NULL; p = p->ai_next) {
		server->fd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol);
		if (server->fd == -1) {
			perror("socket");
			continue;
		}

		status = setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &t,
			sizeof(t));
		if (status == -1) {
			close(server->fd);
			perror("setsockopt");
			continue;
		}

		if (bind(server->fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(server->fd);
			perror("bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "Unable to bind to any socket.\n");
		return -1;
	}

	freeaddrinfo(res);

	if (listen(server->fd, server->fdsz) == -1) {
		perror("listen");
		return -1;
	}

	server->fds[0].fd = server->fd;
	server->fds[0].events = POLLIN;
	++server->nfds;

	return 0;
}

int
run_server(struct httpserver *server, struct dict *dict)
{
	int r = poll(server->fds, server->nfds, -1);
	if (r == -1) {
		perror("poll");
		return -1;
	}

	for (size_t i = 0; i < server->nfds; ++i) {
		if (server->fds[i].revents & POLLIN) {
			if (accept_client(server) == -1) {
				fprintf(stderr,
					"kvatch: unable to connect client\n");
			}
			continue;
		}

		struct request req = parse_request(server->fds[i].fd);
		switch (req.method) {
		case METHOD_ERR:
			resp_code(STATUS_NOT_IMPLEMENTED);
			break;
		case METHOD_GET:;
			char *value = dict_get(dict, req.uri + 1);
			if (value == NULL) {
				resp_code(STATUS_NOT_FOUND);
			} else {
				char body[BUFSIZ] = { 0 };
				snprintf(body, BUFSIZ, "{'%s': '%s'}\n",
					req.uri + 1,
					value);
				respond(server->fds[i].fd, STATUS_OK, body);
			}
			break;
		case METHOD_PUT:
			if (req.body == NULL || req.body[0] == '\0') {
				resp_code(STATUS_BAD_REQUEST);
			} else {
				dict_add(dict, req.uri + 1, req.body);
				resp_code(STATUS_CREATED);
			}
			break;
		case METHOD_DEL:
			dict_del(dict, req.uri + 1);
			resp_code(STATUS_NO_CONTENT);
			break;
		}

		close(server->fds[i].fd);
		free_request(&req);
		server->fds[i] = server->fds[--server->nfds];
	}

	return 0;
}

void
free_server(struct httpserver *server)
{
	for (size_t i = 0; i < server->nfds; ++i) {
		close(server->fds[i].fd);
	}
	free(server->fds);
}

int
accept_client(struct httpserver *server)
{
	int clientfd;
	struct sockaddr_storage clientaddr;
	socklen_t clientaddrlen = sizeof(struct sockaddr);

	clientfd = accept(server->fd, (struct sockaddr *)&clientaddr,
		&clientaddrlen);
	if (clientfd == -1) {
		perror("accept");
		return -1;
	}

	if (server->nfds >= server->fdsz) {
		nfds_t fdsz = server->fdsz * 2;
		struct pollfd *fds = realloc(server->fds,
			sizeof(struct pollfd) * fdsz);
		if (fds == NULL) {
			perror("realloc");
			return -1;
		}

		server->fdsz = fdsz;
		server->fds = fds;
	}

	server->fds[server->nfds].fd = clientfd;
	server->fds[server->nfds].events = POLLIN;
	++server->nfds;

	return clientfd;
}

void
respond(int clientfd, enum httpstatus status, char *body)
{
	char strstatus[32] = { 0 };
	char buf[BUFSIZ] = { 0 };

	switch (status) {
	case STATUS_OK:
		format_status("OK");
		break;
	case STATUS_CREATED:
		format_status("Created");
		break;
	case STATUS_NO_CONTENT:
		format_status("No Content");
		break;
	case STATUS_BAD_REQUEST:
		format_status("Bad Request");
		break;
	case STATUS_NOT_FOUND:
		format_status("Not Found");
		break;
	case STATUS_NOT_IMPLEMENTED:
		format_status("Not Implemented");
		break;
	}

	snprintf(buf, BUFSIZ, "HTTP/1.0 %s\r\n\r\n%s", strstatus, body);
	write(clientfd, buf, strlen(buf));
}
