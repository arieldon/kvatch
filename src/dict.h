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
	void *value;
};

struct dict {
	size_t size;
	size_t capacity;
	struct entry *entries;
};

/* Create and construct a new dictionary. */
struct dict *dict_create(void);

/* Free dictionary and its entries the heap. */
void dict_destroy(struct dict *d);

/* Enter a new key-value pair into the dictionary. */
struct entry *dict_add(struct dict *d, const char *key, void *value);

/* Retrieve a value given its key. */
void *dict_get(struct dict *d, const char *key);

/* Remove and return an existing key-value pair from the dictionary. */
void *dict_del(struct dict *d, const char *key);


/* Use FNV-1a hash function for interaction with dictionary. */
uint32_t _dict_hash(const char *key);

/* Find either an entry by key or an empty space. */
struct entry *_dict_find(struct entry *entries, size_t capacity,
	const char *key);

/* Set key and value of a new entry or modify value of an existing entry. */
struct entry *_dict_set(struct entry *entries, size_t *size, size_t capacity,
	const char *key, void *value);

/* Expand the capacity of the dictionary to reduce its load factor. */
void _dict_expand(struct dict *d);

#endif
