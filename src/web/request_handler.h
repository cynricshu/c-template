// Copyright 2019 Baidu Inc. All Rights Reserved.

#pragma once

#include <baidu/rpc/controller.h>

#include "common/request_context.h"

namespace web {

/**
 * 生成一个随机的 request_id
 */
std::string generate_request_id();

/**
 * 先从 request http header 中寻找 request_id
 * 再从 request http body 中寻找 request_id
 * 如果都没有，则调用 generate_request_id() 方法生成随机 request_id
 */
RequestContext parse_request_id(baidu::rpc::Controller *cntl,
                                const std::string &request_id_in_body);

} // namespace web


