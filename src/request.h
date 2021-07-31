#ifndef REQUEST_H
#define REQUEST_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "dict.h"

enum httpmethod {
	METHOD_ERR,
	METHOD_GET,
	METHOD_PUT,
	METHOD_DEL,
};

struct request {
	enum httpmethod method;
	char *uri;
	char *version;
	struct dict *header;
	char *body;
};

struct request parse_request(int clientfd);
void free_request(struct request *request);

#endif
