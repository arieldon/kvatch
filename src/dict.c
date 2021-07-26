#include "dict.h"

struct dict *
dict_create(void)
{
	struct dict *d = malloc(sizeof(struct dict));
	if (d == NULL) {
		fprintf(stderr, "dict_create: unable to allocate dict\n");
		return NULL;
	}

	d->size = 0;
	d->capacity = DICT_INITIAL_CAPACITY;

	d->entries = calloc(d->capacity, sizeof(struct entry));
	if (d->entries == NULL) {
		free(d);
		return NULL;
	}

	return d;
}

void
dict_destroy(struct dict *d)
{
	for (size_t i = 0; i < d->capacity; ++i) {
		if (d->entries[i].key != NULL) {
			free(d->entries[i].key);
		}
	}

	free(d->entries);
	free(d);
}

struct entry *
dict_add(struct dict *d, const char *key, void *value)
{
	if (d->size + 1 > d->capacity * DICT_MAX_LOAD_CAPACITY) {
		_dict_expand(d);
	}

	return _dict_set(d->entries, &d->size, d->capacity, key, value);
}

void *
dict_get(struct dict *d, const char *key)
{
	assert(key != NULL);

	if (d->size == 0) {
		return NULL;
	}

	return _dict_find(d->entries, d->capacity, key)->value;
}

void *
dict_del(struct dict *d, const char *key)
{
	assert(key != NULL);

	if (d->size == 0) {
		return NULL;
	}

	void *value = NULL;
	struct entry *entry = _dict_find(d->entries, d->capacity, key);

	if (strcmp(key, entry->key) == 0) {
		value = entry->value;

		/* Replace entry with tombstone. */
		free(entry->key);
		entry->key = NULL;
		*(int *)entry->value = 1;
	}

	return value;
}

uint32_t
_dict_hash(const char *key)
{
	uint32_t hash = HASH_FNV_OFFSET_BASIS;

	/* Assume key is null-terminated. */
	for (const char *p = key; *p != '\0'; ++p) {
		hash ^= (uint32_t)(*p);
		hash *= HASH_FNV_PRIME;
	}

	return hash;
}

struct entry *
_dict_find(struct entry *entries, size_t capacity, const char *key)
{
	struct entry *entry;
	struct entry *tombstone = NULL;
	size_t index = _dict_hash(key) % capacity;

	/*
	 * Because the hash table automatically expands, there will be at least
	 * some null value in the table to stop the loop.
	 */
	for (;;) {
		entry = &entries[index];

		if (entry->key == NULL) {
			if (entry->value == NULL) {
				return tombstone != NULL ? tombstone : entry;
			} else if (tombstone == NULL) {
				tombstone = entry;
			}
		} else if (strcmp(key, entry->key) == 0) {
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

struct entry *
_dict_set(struct entry *entries, size_t *size, size_t capacity,
	const char *key, void *value)
{
	assert(key != NULL && value != NULL);

	struct entry *entry = _dict_find(entries, capacity, key);

	if (entry->key == NULL) {
		entry->key = strdup(key);
		if (entry->key == NULL) {
			return NULL;
		}

		++(*size);
	}
	entry->value = value;

	return entry;
}

void
_dict_expand(struct dict *d)
{
	size_t size = 0;
	size_t capacity = d->capacity * 2;
	struct entry *entry;

	if (capacity < d->capacity) {
		return;
	}

	struct entry *entries = calloc(capacity, sizeof(struct entry));
	if (d->entries == NULL) {
		return;
	}

	for (size_t i = 0; i < d->capacity; ++i) {
		entry = &d->entries[i];
		if (entry->key != NULL) {
			_dict_set(entries, &size, capacity, entry->key,
				entry->value);
			free(entry->key);
		}
	}

	d->size = size;
	d->capacity = capacity;

	free(d->entries);
	d->entries = entries;
}
