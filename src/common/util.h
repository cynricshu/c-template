// Copyright 2019 Baidu Inc. All Rights Reserved.

#pragma once

#include <string>
#include <vector>
#include <functional>
#include "request_context.h"

namespace common {

const std::string X_BCE_REQUEST_ID = "trace-id";

int read_wave_from_bytes(
    piat::common::RequestContext &request_context,
    const std::string &audio_data,
    std::vector<float> &wave);

int init_linglong(unsigned int ak, const std::string &sk, const std::string &sign,
                  const std::string &lic_filepath, const std::string &raw_lic_filepath,
                  std::function<void()> callback);

} // namespace common


