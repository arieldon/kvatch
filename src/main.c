#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>

#include "serve.h"

static void
usage(void)
{
	fprintf(stderr, "usage: kvatch [-p PORT]\n");
}

int
main(int argc, char **argv)
{
	char *port = "4000";

	int c;
	while ((c = getopt(argc, argv, "p:h:")) != -1) {
		switch (c) {
		case 'p':
			port = optarg;
			break;
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
		default:
			fprintf(stderr, "kvatch: unable to recognize option\n");
			usage();
			exit(EXIT_FAILURE);
		}
	}
	if (optind < argc) {
		usage();
		exit(EXIT_FAILURE);
	}

	struct httpserver server = { 0 };
	if (init_server(&server, port) == -1) {
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
