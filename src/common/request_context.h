// Copyright 2019 Baidu Inc. All Rights Reserved.

#pragma once

#include <string>

namespace common {

/**
 * 用来记录一次请求中，通用的一些信息
 */
struct RequestContext {

public:
    std::string request_id; // 请求唯一id

    int error_code;

    std::string error_msg;

    explicit RequestContext(const std::string &request_id)
        : request_id(request_id), error_code(0), error_msg("success") {}

    std::string get_request_id_for_log() const {
        return "[" + request_id + "] ";
    }
};

} // namespace common

