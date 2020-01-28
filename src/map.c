#include <map.h>
#include <stdlib.h>
#include <string.h>

void map_init(map_t *m) {
	m->start = m->end = malloc(sizeof *m->start);
	m->size = 0;
}

void map_insert(map_t *m, char *key, void *value) {
	m->end->key = key;
	m->end->value = value;
	m->end->next = malloc(sizeof *m->end->next);
	m->end = m->end->next;
	m->size++;
}

void map_set(map_t *m, char *key, void *value) {
	for (struct map_node_t *node = m->start; node != m->end; node = node->next) {
		if (strcmp(node->key, key) == 0) {
			node->value = value;
			return;
		}
	}

	map_insert(m, key, value);
}

void *map_try_get(map_t *m, char *key, void *default_value) {
	if (key == NULL) {
		return default_value;
	}

	for (struct map_node_t *node = m->start; node != m->end; node = node->next) {
		if (strcmp(node->key, key) == 0) {
			return node->value;
		}
	}

	return default_value;
}

void map_remove(map_t *m, char *key) {
	struct map_node_t *prev, *node;

	if (m->end == m->start->next) {
		memset(m->start, 0, sizeof *m->start);
		return;
	}

	for (node = m->start; node != m->end; prev = node, node = node->next) {
		if (strcmp(node->key, key) == 0) {
			prev->next = node->next;
			free(node);
			node = prev->next;
			break;
		}
	}
}

void map_deinit(map_t *m) {
	struct map_node_t *next, *node;
	for (node = m->start; node != m->end; ) {
		next = node->next;
		free(node);
		node = next;
	}
}
