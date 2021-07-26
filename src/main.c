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
			fprintf(stderr,
				"accept_client: unable to acccept client.");
			run = false;
		}

		struct bundle *bundle = parse_request(clientfd);
		if (bundle == NULL) {
			send_response(clientfd, "ERR\n");
			close(clientfd);
			continue;
		}

		switch (bundle->code) {
		case OP_ERR:
			break;
		case OP_ADD:
			dict_add(dict, bundle->dictentry->key, bundle->dictentry->value);
			break;
		case OP_GET:
			value = (char *)dict_get(dict, bundle->dictentry->key);
			send_response(clientfd, value);
			break;
		case OP_DEL:
			dict_del(dict, bundle->dictentry->key);
			break;
		}

		send_response(clientfd, ok ? "OK\n" : "ERR\n");

		freebundle(bundle);
		close(clientfd);
	} while (run);

	exit(EXIT_SUCCESS);
}
