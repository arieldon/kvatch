#include "parse.h"

enum httpmethod
parse_method(char *str)
{
	size_t len = strlen(str);
	for (size_t i = 0; i < len; ++i) {
		str[i] = toupper(str[i]);
	}

	if (strncmp(str, "GET", len) == 0) {
		return METHOD_GET;
	} else if (strncmp(str, "PUT", len) == 0) {
		return METHOD_PUT;
	} else if (strncmp(str, "DELETE", len) == 0) {
		return METHOD_DEL;
	}

	return METHOD_ERR;
}

static char *
trim_spaces(char *str)
{
	if (str == NULL) return NULL;

	size_t len = strlen(str);
	size_t trailing_spaces = 0;

	for (ssize_t i = len - 1; i >= 0; --i) {
		if (!isspace(str[i])) {
			break;
		}
		++trailing_spaces;
	}
	str[len - trailing_spaces] = 0;

	return str + strspn(str, " \f\n\r\t\v");
}

struct request
parse_request(int clientfd)
{
	struct request req = { 0 };

	char buf[BUFSIZ];
	ssize_t n = read(clientfd, buf, BUFSIZ - 1);
	if (n == -1) {
		perror("read");
		return req;
	}

	char *key, *value;
	char *pos, *pos_prime;
	char *line = strtok_r(buf, "\n", &pos);

	req.method = parse_method(strtok_r(line, " ", &pos_prime));
	if (req.method == METHOD_ERR) {
		return req;
	}
	req.uri = strtok_r(NULL, " ", &pos_prime);
	req.version = trim_spaces(strtok_r(NULL, " ", &pos_prime));

	req.header = dict_create();
	while ((line = trim_spaces(strtok_r(NULL, "\n", &pos))) != NULL && line[0] != 0) {
		key = strtok_r(line, ":", &pos_prime);
		value = strtok_r(NULL, "", &pos_prime);
		dict_add(req.header, trim_spaces(key), trim_spaces(value));
	}

	req.body = strtok_r(NULL, "", &pos);

	return req;
}

void
free_request(struct request *req)
{
	dict_destroy(req->header);
}
