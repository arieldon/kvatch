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
	bool ok, run = true;
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
				ok = true;

				if (fds[i].fd == serverfd) {
					clientfd = accept_client(serverfd);
					if (clientfd == -1) {
						fprintf(stderr, "kvatch: unable to acccept client.");
						run = false;
					}

					if (nfds >= fdsz) {
						nfds_t fdsz_prime = fdsz * 2;
						struct pollfd *fds_prime = realloc(fds,
								sizeof(struct pollfd) * fdsz_prime);
						if (fds_prime == NULL) {
							perror("realloc");
							run = false;
							break;
						}

						fdsz = fdsz_prime;
						fds = fds_prime;
					}

					fds[nfds].fd = clientfd;
					fds[nfds].events = POLLIN;
					++nfds;

					continue;
				}

				struct unit *unit = parse_request(fds[i].fd);
				if (unit == NULL) {
					send_response(fds[i].fd, "ERR\n");
					close(fds[i].fd);
					fds[i] = fds[--nfds];
					continue;
				}

				switch (unit->code) {
				case OP_ERR:
					break;
				case OP_ADD:
					if (dict_add(dict, unit->key, unit->value) == NULL) {
						fprintf(stderr, "kvatch: unable to add entry");
						ok = false;
					}
					break;
				case OP_GET:
					value = (char *)dict_get(dict, unit->key);
					if (value != NULL && isprint(value[0])) {
						send_response(fds[i].fd, value);
					} else {
						fprintf(stderr, "kvatch: key does not exist");
						ok = false;
					}
					break;
				case OP_DEL:
					dict_del(dict, unit->key);
					break;
				}

				send_response(fds[i].fd, ok ? "OK\n" : "ERR\n");
				freeunit(unit);
				close(fds[i].fd);
				fds[i] = fds[--nfds];
			}
		}
	} while (run);

	exit(EXIT_SUCCESS);
}
