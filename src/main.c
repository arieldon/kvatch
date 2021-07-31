#include <stdio.h>
#include <stdlib.h>

#include "serve.h"

int
main(void)
{
	struct httpserver server = { 0 };
	if (init_server(&server) == -1) {
		fprintf(stderr, "kvatch: unable to initialize server\n");
		exit(EXIT_FAILURE);
	}

	struct dict *dict = dict_create();
	if (dict == NULL) {
		fprintf(stderr, "kvatch: unable to initialize hash table\n");
		free_server(&server);
		exit(EXIT_FAILURE);
	}

	while (run_server(&server, dict) == 0);

	exit(EXIT_SUCCESS);
}
