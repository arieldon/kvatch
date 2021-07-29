#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <poll.h>

#include "dict.h"
#include "parse.h"
#include "serve.h"

int
main(void)
{
	char *value;
	bool run = true;
	int serverfd, clientfd;

	nfds_t nfds = 0;
	nfds_t fdsz = 8;
	struct pollfd *fds = calloc(fdsz, sizeof(struct pollfd));
	if (fds == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	if ((serverfd = init_server(PORT)) == -1) {
		exit(EXIT_FAILURE);
	}

	struct dict *dict = dict_create();
	if (dict == NULL) {
		close(serverfd);
		exit(EXIT_FAILURE);
	}

	fds[0].fd = serverfd;
	fds[0].events = POLLIN;
	++nfds;

	do {
		int r = poll(fds, nfds, -1);
		if (r == -1) {
			perror("poll");
			run = false;
			break;
		}

		for (size_t i = 0; i < nfds; ++i) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == serverfd) {
					clientfd = accept_client(serverfd, &fds, &nfds, &fdsz);
					if (clientfd == -1) {
						fprintf(stderr, "kvatch: unable to acccept client.");
					}
					continue;
				}

				struct request req = parse_request(fds[i].fd);

				switch (req.method) {
				case METHOD_ERR:
					send_response(fds[i].fd, "HTTP/1.0 501 Not Implemented\r\n\r\n");
					break;
				case METHOD_GET:
					if ((value = dict_get(dict, req.uri + 1)) == NULL) {
						send_response(fds[i].fd, "HTTP/1.0 404 Not Found\r\n\r\n");
					} else {
						char buf[BUFSIZ];
						snprintf(buf, BUFSIZ, "HTTP/1.0 200 OK\r\n\r\n{'%s': '%s'}",
								req.uri + 1, value);
						printf("value: %s\n", buf);
						send_response(fds[i].fd, buf);
					}
					break;
				case METHOD_PUT:
					dict_add(dict, req.uri + 1, req.body);
					send_response(fds[i].fd, "HTTP/1.0 204 No Content\r\n\r\n");
					break;
				case METHOD_DEL:
					dict_del(dict, req.uri + 1);
					send_response(fds[i].fd, "HTTP/1.0 200 OK\r\n\r\n");
					break;
				}

				close(fds[i].fd);
				fds[i] = fds[--nfds];
			}
		}
	} while (run);

	exit(EXIT_SUCCESS);
}
