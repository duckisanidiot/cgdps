#include <stdio.h>
#include <pthread.h>
#include <map.h>
#include <mongoc/mongoc.h>
#include <string.h>
#include <microhttpd.h>
#include <gdps.h>

static const char default_response[] = "<html>hello, world</html>\0";
map_t *callback_map;
mongoc_client_t *mongo_client;

static int begin_request_handler(struct lh_ctx_t *ctx, struct lh_con_t *conn) {
	struct lh_rqi_t *ri = httplib_get_request_info(conn);
	
	void *func = map_get(callback_map, ri->local_uri);
	if (func == NULL) {
		httplib_printf(ctx, conn, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n%s", default_response);
	} else {
		struct callback_args args;
		args.mongo_client = mongo_client;
		args.ctx = ctx;
		args.conn = conn;
		((callback_type)func)(&args);
	}

	return 1;
}

int main() {
	callback_map = malloc(sizeof *callback_map);
	map_init(callback_map);
	levels_init(callback_map);
	mongoc_init();
	mongo_client = mongoc_client_new("mongodb://localhost:27017/");
	struct lh_ctx_t *ctx;
	struct lh_clb_t callbacks;

	// List of options. Last element must be NULL.
	struct lh_opt_t options[] = {(struct lh_opt_t){"listening_ports","3000"},{NULL}};

	// Prepare callbacks structure. We have only one callback, the rest are NULL.
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = begin_request_handler;

	// Start the web server.
	ctx = httplib_start(&callbacks, NULL, options);

	// Wait until user hits "enter". Server is running in separate thread.
	// Navigating to http://localhost:8080 will invoke begin_request_handler().
	getchar();

	// Stop the server.
	httplib_stop(ctx);


	map_deinit(callback_map);
	mongoc_client_destroy(mongo_client);
	mongoc_cleanup();
}
