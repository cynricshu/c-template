// Copyright 2019 Baidu Inc. All Rights Reserved.

#include "request_handler.h"

#include <base/logging.h>
#include <uuid/uuid.h>

#include "common/util.h"

namespace web {

std::string generate_request_id() {
    uuid_t uuid = {'\0'};
    uuid_generate(uuid);
    char request_id[36 + 1] = {'\0'};
    uuid_unparse_lower(uuid, request_id);

    return std::string(request_id);
}

RequestContext parse_request_id(baidu::rpc::Controller *cntl,
                                const std::string &request_id_in_body) {
    const std::string *request_id = cntl->http_request().GetHeader(piat::common::X_BCE_REQUEST_ID);

    if (request_id != nullptr) {
        LOG(INFO) << "Got request_id from request header: " << *request_id;
        return RequestContext(*request_id);
    } else if (!request_id_in_body.empty()) {
        LOG(INFO) << "Got request_id from request body: " << request_id_in_body;
        return RequestContext(request_id_in_body);
    } else {
        RequestContext request_context(generate_request_id());
        LOG(INFO) << "Generate request_id: " << request_context.request_id;
        return request_context;
    }
}

} // namespace web