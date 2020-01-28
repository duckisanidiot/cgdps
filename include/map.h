#ifndef __MAP_H_
#define __MAP_H_

#include <stdint.h>
#include <stdlib.h>

struct map_node_t {
	char *key;
	void *value;
	struct map_node_t *next;
};

typedef struct {
	struct map_node_t *start, *end;
	size_t size;
} map_t;

void map_init(map_t *m);
void map_insert(map_t *m, char *key, void *value);
void map_set(map_t *m, char *key, void *value);
void *map_try_get(map_t *m, char *key, void *default_value);
void map_remove(map_t *m, char *key);
void map_deinit(map_t *m);

#define map_get(m, key) map_try_get(m, key, NULL)

#endif//__MAP_H_