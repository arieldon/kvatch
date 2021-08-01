#ifndef DICT_H
#define DICT_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_INITIAL_CAPACITY	16
#define DICT_MAX_LOAD_CAPACITY	0.75

#define HASH_FNV_OFFSET_BASIS	2166136261u
#define HASH_FNV_PRIME		16777619

struct entry {
	char *key;
	char *value;
};

struct dict {
	size_t size;
	size_t capacity;
	struct entry *entries;
};

/* Create and construct a new dictionary. */
struct dict *dict_create(void);

/* Free dictionary and its entries the heap. */
void dict_clear(struct dict *dict);

/* Enter a new key-value pair into the dictionary. */
struct entry *dict_add(struct dict *dict, const char *key, char *value);

/* Retrieve a value given its key. */
char *dict_get(struct dict *dict, const char *key);

/* Remove and return an existing key-value pair from the dictionary. */
char *dict_del(struct dict *dict, const char *key);

#endif
