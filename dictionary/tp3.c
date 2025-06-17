#include "tp3.h"
#include <stdint.h>

#include <string.h>
#include <stdlib.h>

#include <stdio.h>

#define INITIAL_CAPACITY 65537
#define LOAD_FACTOR 0.75 
#define GROWTH_FACTOR 2

typedef struct entry {
  char *key;
  void *value;
  bool deleted;
} entry_t;

struct dictionary {
  entry_t *entries;
  size_t size;
  size_t capacity;
  destroy_f destroy;
};

void add_key(dictionary_t *dictionary, size_t index, char *key, void *value) {
  dictionary->entries[index].key = key;
  dictionary->entries[index].value = value;
  dictionary->entries[index].deleted = false;
  dictionary->size++;
}

void delete_key(dictionary_t *dictionary, size_t index) {
  free(dictionary->entries[index].key); 
  dictionary->entries[index].key = NULL;
  dictionary->entries[index].deleted = true;
  dictionary->size--;
}

size_t find_empty(dictionary_t *dictionary, const char *key) {
  size_t index = murmurhash(key, (uint32_t)strlen(key), 0) % dictionary->capacity;
  while (dictionary->entries[index].key != NULL) {
    index = (index + 1) % dictionary->capacity;
  }
  return index;
}

uint32_t
murmurhash (const char *key, uint32_t len, uint32_t seed) {
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;
  uint32_t h = 0;
  uint32_t k = 0;
  uint8_t *d = (uint8_t *) key; 
  const uint32_t *chunks = NULL;
  const uint8_t *tail = NULL; 
  int i = 0;
  int l = len / 4; 
  h = seed;
  chunks = (const uint32_t *) (d + l * 4); 
  tail = (const uint8_t *) (d + l * 4); 
  for (i = -l; i != 0; ++i) {
    k = chunks[i];
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;
    h ^= k;
    h = (h << r2) | (h >> (32 - r2));
    h = h * m + n;
  }
  k = 0;
  switch (len & 3) { 
    case 3: k ^= (tail[2] << 16);
    case 2: k ^= (tail[1] << 8);
    case 1:
      k ^= tail[0];
      k *= c1;
      k = (k << r1) | (k >> (32 - r1));
      k *= c2;
      h ^= k;
  }
  h ^= len;
  h ^= (h >> 16);
  h *= 0x85ebca6b;
  h ^= (h >> 13);
  h *= 0xc2b2ae35;
  h ^= (h >> 16);
  return h;
}


bool rehash(dictionary_t *dictionary) {
  if (dictionary->size == 0) {
    return false;
  }
  size_t previous_capacity = dictionary->capacity;
  entry_t *previous_entries = dictionary->entries;
  size_t previous_size = dictionary->size;

  size_t new_capacity = GROWTH_FACTOR * dictionary->capacity;
  entry_t *new_entries = (entry_t *)calloc(new_capacity, sizeof(entry_t));
  if (new_entries == NULL) {
    return false;
  }
  dictionary->capacity = new_capacity;
  dictionary->entries = new_entries;
  dictionary->size = 0;

  for (size_t i = 0; i < previous_capacity; i++) {
    if (previous_entries[i].key != NULL && !previous_entries[i].deleted) {
      size_t index = find_empty(dictionary, previous_entries[i].key);
      char *key_copy = (char *) malloc(strlen(previous_entries[i].key) + 1);
      if (key_copy == NULL) {
        free(new_entries);
        dictionary->capacity = previous_capacity;
        dictionary->entries = previous_entries;
        dictionary->size = previous_size;
        return false;
      }
      add_key(dictionary, index, strcpy(key_copy, previous_entries[i].key), previous_entries[i].value);
    }
  }

  for (size_t i = 0; i < previous_capacity; i++) {
    if (previous_entries[i].key != NULL) {
      free(previous_entries[i].key);
    }
  }
  free(previous_entries);
  return true;
}

dictionary_t *dictionary_create(destroy_f destroy) { 
  dictionary_t *dictionary = (dictionary_t *) malloc(sizeof(dictionary_t));
  if (dictionary == NULL) {
    return NULL;
  }
  dictionary->entries = (entry_t *) calloc(INITIAL_CAPACITY, sizeof(entry_t));
  if (dictionary->entries == NULL) {
    free(dictionary);
    return NULL;
  }
  dictionary->size = 0;
  dictionary->capacity = INITIAL_CAPACITY;
  dictionary->destroy = destroy;
  return dictionary;
};

bool dictionary_put(dictionary_t *dictionary, const char *key, void *value) {
  if ((float)dictionary->size / (float)dictionary->capacity >= LOAD_FACTOR) { 
    if (!rehash(dictionary)) {
      return false;
    }
  }

  size_t index = murmurhash(key, (uint32_t)strlen(key), 0) % dictionary->capacity;
  while (dictionary->entries[index].key != NULL) { 
    if (!dictionary->entries[index].deleted && strcmp(dictionary->entries[index].key, key) == 0) {
      free(dictionary->entries[index].key);
      if (dictionary->destroy != NULL) {
        dictionary->destroy(dictionary->entries[index].value);
      }
      dictionary->size--;
      break; 
    }
    index = (index + 1) % dictionary->capacity; 
  } 

  char *key_copy = (char *) malloc(strlen(key) + 1); 
  if (key_copy == NULL) {
    return false;
  }
  add_key(dictionary, index, strcpy(key_copy, key), value);
  return true;
};

void *dictionary_get(dictionary_t *dictionary, const char *key, bool *err) {
  size_t index = murmurhash(key, (uint32_t)strlen(key), 0) % dictionary->capacity;
  while (dictionary->entries[index].key != NULL || dictionary->entries[index].deleted) {
    if (!dictionary->entries[index].deleted && strcmp(dictionary->entries[index].key, key) == 0) {
      *err = false;
      return dictionary->entries[index].value;
    }
    index = (index + 1) % dictionary->capacity;
  }
  *err = true;
  return NULL;
};

bool dictionary_delete(dictionary_t *dictionary, const char *key) { 
  size_t index = murmurhash(key, (uint32_t)strlen(key), 0) % dictionary->capacity;
  while (dictionary->entries[index].key != NULL || dictionary->entries[index].deleted) {
    if (!dictionary->entries[index].deleted && strcmp(dictionary->entries[index].key, key) == 0) {
      delete_key(dictionary, index);
      if (dictionary->destroy != NULL) {
        dictionary->destroy(dictionary->entries[index].value);
        return true;
      }
    }
    index = (index + 1) % dictionary->capacity;
  } 
  return false;
};

void *dictionary_pop(dictionary_t *dictionary, const char *key, bool *err) {
  size_t index = murmurhash(key, (uint32_t)strlen(key), 0) % dictionary->capacity;
  while (dictionary->entries[index].key != NULL || dictionary->entries[index].deleted) {
    if (!dictionary->entries[index].deleted && strcmp(dictionary->entries[index].key, key) == 0) {
      void *value = dictionary->entries[index].value; 
      delete_key(dictionary, index);
      *err = false;
      return value;
    }
    index = (index + 1) % dictionary->capacity;
  } 
  *err = true;
  return NULL;
};

bool dictionary_contains(dictionary_t *dictionary, const char *key) {
  bool err;
  if (dictionary_get(dictionary, key, &err) == NULL) {
    return !err;
  }
  return true;
};

size_t dictionary_size(dictionary_t *dictionary) { return dictionary->size; };

void dictionary_destroy(dictionary_t *dictionary) { 
  for (size_t i = 0; i < dictionary->capacity; i++) {
    if (dictionary->entries[i].key != NULL) {
      free(dictionary->entries[i].key); 
      if (dictionary->destroy != NULL) {
        dictionary->destroy(dictionary->entries[i].value);
      }
    }
  }
  free(dictionary->entries);
  free(dictionary);
};