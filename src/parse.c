#include "parse.h"

static char *
strip_right_spaces(char *str)
{
	size_t trailing_spaces = 0;

	for (ssize_t i = strlen(str); i >= 0; --i) {
		if (!isspace(str[i])) {
			break;
		}
		++trailing_spaces;
	}
	str[strlen(str) - trailing_spaces] = '\0';

	return str;
}

static char *
strip_left_spaces(char *str)
{
	return str + strspn(str, " ");
}

static char *
strip_spaces(char *str)
{
	return strip_right_spaces(strip_left_spaces(str));
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

	strentry = strip_spaces(strentry);

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

	if ((dictentry->key = strndup(strentry, split - strentry)) == NULL) {
		perror("strndup");
		goto fail;
	}
	if ((dictentry->value = strdup(
			strip_spaces(split + strlen(delim)))) == NULL) {
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
		bundle->dictentry = NULL;
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
		free(bundle->dictentry->value);
		free(bundle->dictentry);
	}
	free(bundle);
}
