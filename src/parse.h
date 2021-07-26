#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <ctype.h>
#include <stdbool.h>
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

struct unit {
	enum operation code;
	char *key;
	char *value;
};

enum operation parse_op(char *str);

struct unit *parse_request(int clientfd);
bool entrytok(struct unit *unit, char *strentry, char *delim);

void freeunit(struct unit *unit);

#endif
