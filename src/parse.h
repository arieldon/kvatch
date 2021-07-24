#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "dict.h"

#define BUFSIZE	4096

enum operation {
	OP_ERR,
	OP_ADD,
	OP_GET,
	OP_DEL,
};

struct bundle {
	enum operation code;
	struct entry *dictentry;
};

enum operation parse_op(char *str);

struct bundle *parse_request(int clientfd);
struct entry *entrytok(char *strentry, char *delim);

void freebundle(struct bundle *bundle);

#endif
