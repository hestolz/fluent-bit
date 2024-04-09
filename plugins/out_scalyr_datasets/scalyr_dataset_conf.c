/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2024 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <fluent-bit/flb_output_plugin.h>
#include <fluent-bit/flb_utils.h>

#include "scalyr_dataset.h"
#include "scalyr_dataset_conf.h"

struct flb_scalyr_dataset *flb_scalyr_dataset_conf_create(struct flb_output_instance *ins,
                                          struct flb_config *config)
{
    int ret;
    int io_flags = 0;
    size_t size;
    const char *tmp;
    struct flb_upstream *upstream;
    struct flb_scalyr_dataset *ctx;

    ctx = flb_calloc(1, sizeof(struct flb_scalyr_dataset));
    if (!ctx) {
        flb_errno();
        return NULL;
    }
    ctx->ins = ins;
    mk_list_init(&ctx->fields);

    ret = flb_output_config_map_set(ins, (void *) ctx);
    if (ret == -1) {
        flb_free(ctx);
        return NULL;
    }

    /* Set default network configuration */
    flb_output_net_default(FLB_SCALYR_DATASET_DEFAULT_HOST, FLB_SCALYR_DATASET_DEFAULT_PORT, ins);

    /* use TLS ? */
    if (ins->use_tls == FLB_TRUE) {
        io_flags = FLB_IO_TLS;
    }
    else {
        io_flags = FLB_IO_TCP;
    }

    if (ins->host.ipv6 == FLB_TRUE) {
        io_flags |= FLB_IO_IPV6;
    }

    /* Prepare an upstream handler */
    upstream = flb_upstream_create(config,
                                   ins->host.name,
                                   ins->host.port,
                                   io_flags,
                                   ins->tls);
    if (!upstream) {
        flb_plg_error(ctx->ins, "cannot create Upstream context");
        flb_scalyr_dataset_conf_destroy(ctx);
        return NULL;
    }

    /* Set manual Index and Type */
    ctx->u = upstream;

    tmp = flb_output_get_property("http_buffer_size", ins);
    if (!tmp) {
        ctx->buffer_size = 0;
    }
    else {
        size = flb_utils_size_to_bytes(tmp);
        if (size == -1) {
            flb_plg_error(ctx->ins, "invalid 'buffer_size' value");
            flb_scalyr_dataset_conf_destroy(ctx);
            return NULL;
        }
        if (size < 4 *1024) {
            size = 4 * 1024;
        }
        ctx->buffer_size = size;
    }

    /* Compress (gzip) */
    tmp = flb_output_get_property("compress", ins);
    ctx->compress_gzip = FLB_FALSE;
    if (tmp) {
        if (strcasecmp(tmp, "gzip") == 0) {
            ctx->compress_gzip = FLB_TRUE;
        }
    }

    /* Set instance flags into upstream */
    flb_output_upstream_set(ctx->u, ins);

    return ctx;
}

int flb_scalyr_dataset_conf_destroy(struct flb_scalyr_dataset *ctx)
{
    if (!ctx) {
        return -1;
    }

    if (ctx->auth_header) {
        flb_sds_destroy(ctx->auth_header);
    }
    if (ctx->u) {
        flb_upstream_destroy(ctx->u);
    }

    flb_free(ctx);

    return 0;
}
