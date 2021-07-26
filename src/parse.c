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

struct entry *
entrytok(char *strentry, char *delim)
{
	char *split;
	struct entry *dictentry;

	strentry = stripspaces(strentry);

	dictentry = calloc(1, sizeof(struct entry));
	if (dictentry == NULL) {
		fprintf(stderr, "entrytok: unable to allocate entry\n");
		goto fail;
	}

	split = strstr(strentry, delim);
	if (split == NULL) {
		fprintf(stderr, "entrytok: unable to locate delimeter\n");
		goto fail;
	}

	if ((dictentry->key = strndup(strentry, split - strentry - 1)) == NULL) {
		perror("strndup");
		goto fail;
	}
	if ((dictentry->value = strdup(
			stripspaces(split + strlen(delim)))) == NULL) {
		perror("strdup");
		goto fail;
	}
	return dictentry;
fail:
	free(dictentry->key);
	free(dictentry->value);
	free(dictentry);
	free(split);
	return NULL;
}

struct bundle *
parse_request(int clientfd)
{
	char *buf;
	ssize_t nb;
	enum operation op;
	struct bundle *bundle;

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

	bundle = malloc(sizeof(struct bundle));
	if (bundle == NULL) {
		perror("bundle");
		goto fail;
	}
	bundle->code = op;

	if (op == OP_ADD) {
		if ((bundle->dictentry = entrytok(strtok(NULL, ""), "->")) == NULL) {
			fprintf(stderr, "parse_request: unable to form entry\n");
			goto fail;
		}
	} else {
		bundle->dictentry = calloc(1, sizeof(struct entry));
		if (bundle->dictentry == NULL) {
			fprintf(stderr, "parse_request: unable to form entry\n");
			goto fail;
		}
		bundle->dictentry->key = strdup(stripspaces(strtok(NULL, "")));
	}


	free(buf);
	return bundle;
fail:
	free(buf);
	freebundle(bundle);
	return NULL;
}

void
freebundle(struct bundle *bundle)
{
	if (bundle->dictentry) {
		free(bundle->dictentry->key);
		free(bundle->dictentry);
	}
	free(bundle);
}
