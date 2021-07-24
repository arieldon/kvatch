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
	bool run = true;
	int serverfd, clientfd;

	if ((serverfd = init_server(PORT)) == -1) {
		exit(EXIT_FAILURE);
	}

	do {
		clientfd = accept_client(serverfd);
		if (clientfd == -1) {
			fprintf(stderr,
				"accept_client: unable to acccept client.");
			run = false;
		}

		struct bundle *bundle = parse_request(clientfd);
		if (bundle == NULL) {
			send_response(clientfd, "Unable to process request.\n");
			close(clientfd);
			continue;
		}

		printf("op: %d\n", bundle->code);
		if (bundle->dictentry) {
			printf("key: %s\n", bundle->dictentry->key);
			printf("value: %s\n", (char *)bundle->dictentry->value);
		}

		switch (op) {
		case OP_ADD:
			/* TODO */
			break;
		case OP_GET:
			/* TODO */
			break;
		case OP_DEL:
			/* TODO */
			break;
		}

		send_response(clientfd, "Request successfully processed.\n");

		freebundle(bundle);

		close(clientfd);
	} while (run);

	exit(EXIT_SUCCESS);
}
