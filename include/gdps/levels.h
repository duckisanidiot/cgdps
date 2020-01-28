#ifndef __GDPS_LEVELS_H_
#define __GDPS_LEVELS_H_

#include <map.h>
#include <stdbool.h>

struct gd_level_t {
	int id;
	char *name;
	int star_diff, downloads, likes;
	int song, stars;
	char *description;
	int length, author;
	bool present;
};

void levels_init(map_t *callbacks);

#endif//__GDPS_LEVELS_H_
