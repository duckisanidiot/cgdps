#include <gdps/hash.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include <gdps/levels.h>

static const char hex_alphabet[] = "0123456789abcdef";

int get_last_digit(int x) {
	int d = 0;

	while (x) {
		d = x % 10;
		x /= 10;
		if (x == 0) break;
	}

	return d;
}

void hash_levels(struct gd_level_t *levels, char *out) {
	unsigned level_count = 0;
	char *hash_str;
	char buffer_1[20], buffer_2[3];
	
	for (; levels[level_count].present; ++level_count);

	hash_str = calloc(1, level_count * 13 + 13);
	for (unsigned i = 0, j = 0; i < level_count; ++i) {
		j += sprintf(&hash_str[j], "%d%d%d0", levels[i].id % 10, get_last_digit(levels[i].id), levels[i].stars);
	}
	strcat(hash_str, "xI25fpAapCQg");
	printf("hash str %s\n", hash_str);

	SHA1(hash_str, strlen(hash_str), buffer_1);
	for (unsigned i = buffer_2[2] = 0; i < 20; ++i) {
		buffer_2[0] = hex_alphabet[(unsigned char)(buffer_1[i]) >> 4];
		buffer_2[1] = hex_alphabet[(unsigned char)(buffer_1[i]) & 0xF];
		strcat(out, buffer_2);
	}

	free(hash_str);
}
