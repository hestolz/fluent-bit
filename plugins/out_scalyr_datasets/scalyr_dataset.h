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

#ifndef FLB_OUT_SCALYR_DATASET
#define FLB_OUT_SCALYR_DATASET

#define FLB_SCALYR_DATASET_DEFAULT_HOST          "app.scalyr.com"
#define FLB_SCALYR_DATASET_DEFAULT_PORT          443
#define FLB_SCALYR_DATASET_DEFAULT_URI           "/api/addEvents"
#define FLB_SCALYR_DATASET_DEFAULT_HTTP_MAX      "2M"

#define FLB_SCALYR_DATASET_DEFAULT_TIME           "ts"
#define FLB_SCALYR_DATASET_DEFAULT_EVENT_ENVELOPE "attrs"

#include <fluent-bit/flb_output_plugin.h>
#include <fluent-bit/flb_sds.h>
#include <fluent-bit/flb_record_accessor.h>

struct flb_scalyr_dataset_field {
    flb_sds_t key_name;
    struct flb_record_accessor *ra;
    struct mk_list _head;
};

struct flb_scalyr_dataset {
    /* Payload compression */
    int compress_gzip;

    /* Internal/processed event fields */
    struct mk_list fields;

    /* Bearer Token */
    flb_sds_t bearer_token;

    /* Token Auth */
    flb_sds_t auth_header;

    /* HTTP Client Setup */
    size_t buffer_size;

    /* HTTP: Debug bad requests (HTTP status 400) to stdout */
    int http_debug_bad_request;

    /* Upstream connection to the backend server */
    struct flb_upstream *u;

    /* Plugin instance */
    struct flb_output_instance *ins;
};

#endif
