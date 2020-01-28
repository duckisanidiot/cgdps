#include <gdps/levels.h>
#include <map.h>
#include <gdps.h>
#include <mongoc/mongoc.h>
#include <stdbool.h>
#include <gdps/hash.h>
#include <stdlib.h>

#define LEVELS_PER_PAGE 10

bool is_string_numeric(const char *s) {
	while (*s++) {
		if (*s < '0' || *s > '9') {
			return false;
		}
	}

	return true;
}

// TODO: sort_by
struct gd_level_t *get_levels_from_db(mongoc_client_t *client, const char *search_query, int page/*, int sort_by*/) {
	mongoc_collection_t *levels_col = mongoc_client_get_collection(client, "cgdps", "levels");
	struct gd_level_t *levels;
	bson_t *opts, *filter;
	size_t _;
	const bson_t *level_doc;
	mongoc_cursor_t *cursor;
	bson_iter_t iter;

	if (*search_query == '\0') {
		filter = bson_new();
	} else if (!is_string_numeric(search_query)) {
		filter = BCON_NEW("name", "{", "$regex", BCON_UTF8(search_query), "}");
	} else {
		filter = BCON_NEW("id", BCON_INT32(atoi(search_query)));
	}

	opts = BCON_NEW(
		"skip", BCON_INT64(page * LEVELS_PER_PAGE),
		"limit", BCON_INT64(LEVELS_PER_PAGE),
		"sort", "{",
			"likes", BCON_INT32(-1),
		"}"
	);

	levels = calloc(LEVELS_PER_PAGE, sizeof *levels);
	cursor = mongoc_collection_find_with_opts(levels_col, filter, opts, NULL);
	for (unsigned i = 0; mongoc_cursor_next(cursor, &level_doc); ++i) {
#define get_item(type, field, ...) (bson_iter_init_find(&iter, level_doc, field), bson_iter_##type(&iter, ##__VA_ARGS__))
		levels[i].id = get_item(int32, "id");
		levels[i].name = bson_strdup(get_item(utf8, "name", &_));
		levels[i].star_diff = get_item(int32, "diff");
		levels[i].downloads = get_item(int32, "downloads");
		levels[i].likes = get_item(int32, "likes");
		levels[i].song = get_item(int32, "song");
		levels[i].stars = get_item(int32, "stars");
		levels[i].description = bson_strdup(get_item(utf8, "description", &_));
		levels[i].length = get_item(int32, "length");
		levels[i].author = get_item(int32, "author");
		levels[i].present = true;
#undef get_item
	}

	bson_destroy(opts);
	bson_destroy(filter);
	mongoc_cursor_destroy(cursor);
	mongoc_collection_destroy(levels_col);

	return levels;
}

void getGJLevels21(struct callback_args *args) {
	struct gd_level_t *levels;
	char *post_data = malloc(4096);
	char *search_query = malloc(128);
	char *page_str = malloc(16);
	char *results = malloc(65536);
	char *buffer = malloc(4096);
	int level_count = 0;
	size_t post_data_len = httplib_read(args->ctx, args->conn, post_data, 4096);
	httplib_get_var(post_data, post_data_len, "str", search_query, 128);
	httplib_get_var(post_data, post_data_len, "page", page_str, 16);

	levels = get_levels_from_db(args->mongo_client, search_query, atoi(page_str));

	for (unsigned i = 0; levels[i].present; ++i) {
		// oh fuck you robtop
		sprintf(
			buffer,
			"1:%d:2:%s:5:1:6:%d:8:10:9:%d:10:%d:12:%d:13:21:14:%d:17:0:43:0:25:0:18:%d:19:%d:45:10:3:%s:15:%d:30:0:31:0:37:0:38:0:39:0:46:1:47:2:40:0:35:0|",
			levels[i].id, levels[i].name, levels[i].author, levels[i].star_diff, levels[i].downloads, levels[i].song,
			levels[i].likes, levels[i].stars, levels[i].description, 0, levels[i].length
		);
		strcat(results, buffer);
		level_count++;
	}
	results[strlen(results) - 1] = '#';
	
	for (unsigned i = 0; levels[i].present; ++i) {
		sprintf(buffer, "%d:UserID%d:0|", levels[i].author, levels[i].author);
		strcat(results, buffer);
	}
	results[strlen(results) - 1] = '#';

	/*
	for (unsigned i = 0; levels[i].present; ++i) {
		// TODO: songs
	}
	*/
	strcat(results, "#");

	sprintf(buffer, "%d:0:10#", level_count);
	strcat(results, buffer);
	buffer[0] = 0;

	hash_levels(levels, buffer);
	strcat(results, buffer);

	httplib_printf(args->ctx, args->conn, "HTTP/1.1 200 OK\r\n\r\n%s", results);

	//free(results);
	free(buffer);
	free(search_query);
	free(page_str);
	free(post_data);
	
	for (unsigned i = 0; levels[i].present; ++i) {
		free(levels[i].description);
		free(levels[i].name);
	}
	free(levels);
}

void levels_init(map_t *callbacks) {
	map_insert(callbacks, "/aa/database/getGJLevels21.php", getGJLevels21);
}
