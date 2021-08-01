#include "dict.h"

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
	const char *key, char *value)
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
	entry->value = strdup((char *)value);

	return entry;
}

void
_dict_expand(struct dict *dict)
{
	size_t capacity = dict->capacity * 2;
	struct entry *entry;

	if (capacity < dict->capacity) {
		return;
	}

	struct entry *entries = calloc(capacity, sizeof(struct entry));
	if (dict->entries == NULL) {
		return;
	}

	for (size_t i = 0; i < dict->capacity; ++i) {
		entry = &dict->entries[i];
		if (entry->key != NULL) {
			_dict_set(entries, &dict->size, capacity, entry->key,
				entry->value);
			free(entry->key);
		}
	}

	dict->capacity = capacity;

	free(dict->entries);
	dict->entries = entries;
}

struct dict *
dict_create(void)
{
	struct dict *dict = malloc(sizeof(struct dict));
	if (dict == NULL) {
		fprintf(stderr, "dict_create: unable to allocate dict\n");
		return NULL;
	}

	dict->size = 0;
	dict->capacity = DICT_INITIAL_CAPACITY;

	dict->entries = calloc(dict->capacity, sizeof(struct entry));
	if (dict->entries == NULL) {
		free(dict);
		return NULL;
	}

	return dict;
}

void
dict_clear(struct dict *dict)
{
	for (size_t i = 0; i < dict->capacity; ++i) {
		if (dict->entries[i].key != NULL) {
			free(dict->entries[i].key);
			free(dict->entries[i].value);
		}
	}
	free(dict->entries);
	free(dict);
}

struct entry *
dict_add(struct dict *dict, const char *key, char *value)
{
	if (dict->size + 1 > dict->capacity * DICT_MAX_LOAD_CAPACITY) {
		_dict_expand(dict);
	}
	return _dict_set(dict->entries, &dict->size, dict->capacity, key, value);
}

char *
dict_get(struct dict *dict, const char *key)
{
	assert(key != NULL);

	if (dict->size == 0) {
		return NULL;
	}

	return _dict_find(dict->entries, dict->capacity, key)->value;
}

char *
dict_del(struct dict *dict, const char *key)
{
	assert(key != NULL);

	if (dict->size == 0) {
		return NULL;
	}

	char *value = NULL;
	struct entry *entry = _dict_find(dict->entries, dict->capacity, key);

	if (entry->key != NULL && strcmp(key, entry->key) == 0) {
		value = entry->value;

		free(entry->key);
		free(entry->value);
		entry->key = NULL;
		entry->value = NULL;

		--dict->size;
	}

	return value;
}
