#ifndef __GDPS_H_
#define __GDPS_H_

#include <mongoc/mongoc.h>
#include <libhttp.h>

struct callback_args {
	mongoc_client_t *mongo_client;
	struct lh_ctx_t *ctx;
	struct lh_con_t *conn;
};

typedef void(*callback_type)(struct callback_args *);

#include "gdps/levels.h"

#endif//__GDPS_H_
