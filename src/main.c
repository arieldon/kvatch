#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "parse.h"
#include "serve.h"

int
main(void)
{
	char *value;
	bool ok, run = true;
	int serverfd, clientfd;

	if ((serverfd = init_server(PORT)) == -1) {
		exit(EXIT_FAILURE);
	}

	struct dict *dict = dict_create();
	if (dict == NULL) {
		close(serverfd);
		exit(EXIT_FAILURE);
	}

	do {
		ok = true;

		clientfd = accept_client(serverfd);
		if (clientfd == -1) {
			fprintf(stderr, "kvatch: unable to acccept client.");
			run = false;
		}


		struct unit *unit = parse_request(clientfd);
		if (unit == NULL) {
			send_response(clientfd, "ERR\n");
			close(clientfd);
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
				send_response(clientfd, value);
			} else {
				fprintf(stderr, "kvatch: key does not exist");
				ok = false;
			}
			break;
		case OP_DEL:
			dict_del(dict, unit->key);
			break;
		}

		send_response(clientfd, ok ? "OK\n" : "ERR\n");

		freeunit(unit);
		close(clientfd);
	} while (run);

	exit(EXIT_SUCCESS);
}
