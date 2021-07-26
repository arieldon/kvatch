#include "parse.h"

static char *
stripspaces(char *str)
{
	size_t trailing_spaces = 0;

	/* Strip trailing spaces. */
	for (ssize_t i = strlen(str) - 1; i >= 0; --i) {
		if (!isspace(str[i])) {
			break;
		}
		++trailing_spaces;
	}
	str[strlen(str) - trailing_spaces] = '\0';

	/* Strip leading spaces. */
	str = str + strspn(str, " \f\n\r\t\v");

	return str;
}

enum operation
parse_op(char *str)
{
	size_t len = strlen(str);
	for (size_t i = 0; i < len; ++i) {
		str[i] = toupper(str[i]);
	}

	if (strncmp(str, "ADD", len) == 0) {
		return OP_ADD;
	} else if (strncmp(str, "GET", len) == 0) {
		return OP_GET;
	} else if (strncmp(str, "DEL", len) == 0) {
		return OP_DEL;
	}

	return OP_ERR;
}

bool
entrytok(struct unit *unit, char *strentry, char *delim)
{
	char *split;

	strentry = stripspaces(strentry);

	split = strstr(strentry, delim);
	if (split == NULL) {
		fprintf(stderr, "entrytok: unable to locate delimeter\n");
		goto fail;
	}

	if ((unit->key = strndup(strentry, split - strentry - 1)) == NULL) {
		perror("strndup");
		goto fail;
	}
	if ((unit->value = strdup(
			stripspaces(split + strlen(delim)))) == NULL) {
		perror("strdup");
		goto fail;
	}
	return true;
fail:
	free(unit->key);
	free(unit->value);
	free(split);
	return false;
}

struct unit *
parse_request(int clientfd)
{
	char *buf;
	ssize_t nb;
	enum operation op;
	struct unit *unit;

	buf = calloc(BUFSIZE, sizeof(char));
	if ((nb = read(clientfd, buf, BUFSIZE - 1)) == -1) {
		perror("read");
		goto fail;
	}
	puts(buf);

	if ((op = parse_op(strtok(buf, ":"))) == OP_ERR) {
		fprintf(stderr, "parse_request: unable to parse operator\n");
		goto fail;
	}

	unit = malloc(sizeof(struct unit));
	if (unit == NULL) {
		perror("unit");
		goto fail;
	}
	unit->code = op;

	if (op == OP_ADD) {
		entrytok(unit, strtok(NULL, ""), "->");
		if (unit->key == NULL || unit->value == NULL) {
			fprintf(stderr, "parse_request: unable to form entry\n");
			goto fail;
		}
	} else {
		unit->key = strdup(stripspaces(strtok(NULL, "")));
		if (unit->key == NULL) {
			perror("strdup");
			goto fail;
		}
	}


	free(buf);
	return unit;
fail:
	free(buf);
	freeunit(unit);
	return NULL;
}

void
freeunit(struct unit *unit)
{
	/* Value is intentionally left unfree. */
	free(unit->key);
	free(unit);
}
