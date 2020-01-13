// Copyright 2019 Baidu Inc. All Rights Reserved.

#include "util.h"

#include <base/logging.h>
#include <base/base64.h>
#include <base/file_util.h>
#include <uuid/uuid.h>
#include <feat/wave-reader.h>

#include "base64.h"
#include "web/audio_data_handler.h"

namespace common {

int handle_wave_data(piat::common::RequestContext &request_context,
                     const kaldi::WaveData &wave_data,
                     std::vector<float> &wave_vector) {

    kaldi::Matrix<float> data(wave_data.Data());

    if (data.NumRows() != 1) {
        request_context.error_msg = "wave date error, the num rows is "
            + std::to_string(data.NumRows());
        LOG(WARNING) << request_context.get_request_id_for_log()
                     << request_context.error_msg;
        return -1;
    }
    wave_vector.resize(data.NumCols());
    for (int i = 0; i < data.NumCols(); ++i) {
        wave_vector[i] = data.Data()[i];
    }

    return 0;
}

int read_wave_from_bytes(piat::common::RequestContext &request_context,
                         const std::string &audio_data,
                         std::vector<float> &wave_vector) {
    LOG(NOTICE) << request_context.get_request_id_for_log()
                << "start to get vector from audio data";

    std::istringstream audio_stream(audio_data);

    kaldi::WaveData wave_data;
    try {
        wave_data.Read(audio_stream);
    } catch (std::exception &e) {
        request_context.error_msg = "Failed to Read WaveData, Invalid audio file";
        LOG(WARNING) << request_context.get_request_id_for_log()
                     << request_context.error_msg
                     << e.what();
        return -1;
    }

    return handle_wave_data(request_context, wave_data, wave_vector);
}

int init_linglong(unsigned int ak, const std::string &sk, const std::string &sign,
                  const std::string &lic_filepath, const std::string &raw_lic_filepath,
                  std::function<void()> callback) {
    LOG(NOTICE) << "start to init linglong security check";

    // raw license 是加密狗能直接读入的。 license 经过 base64 decode后，得到 raw license。
    // 正常情况下，分发出去的都是license。只有特殊情况下，才会分发raw license。
    if (!base::PathExists(base::FilePath(raw_lic_filepath))) {
        if (!base::PathExists(base::FilePath(lic_filepath))) {
            // license 与 raw license 都不存在，直接报错
            LOG(ERROR) << "license file not exists!";
            return -1;
        } else {
            // license 存在，decode 得到 raw license
            if (0 != piat::common::decode(lic_filepath.data(), raw_lic_filepath.data())) {
                LOG(ERROR) << "failed to decode license file";
                return -1;
            }
        }
    }

    Dongle *dongle = Dongle::get_instance();
    dongle->guard(callback, ak, sk, sign, raw_lic_filepath);

    LOG(NOTICE) << "finish to init linglong security check";
    return 0;
}

#ifdef AIPE_SECURITY
int auth_check(piat::common::RequestContext &request_context,
               std::string product_name,
               std::string container_id) {
    aipe_auth_request auth_request;
    auth_request.product_name = const_cast<char *> (product_name.c_str());
    auth_request.container_id = const_cast<char *> (container_id.c_str());
    //鉴权响应结果，如果response.result为0表示成功；其他失败
    aipe_auth_response auth_response = check_auth(auth_request);

    if (0 != auth_response.result) {
        LOG(ERROR) << "Failed to check auth, result from security server is: "
            + std::to_string(auth_response.result);
        request_context.error_msg = "Failed to check auth, cannot use this api";
    }

    return auth_response.result;
}
#endif

std::string generate_request_id() {
    uuid_t uuid = {'\0'};
    uuid_generate(uuid);
    char request_id[36 + 1] = {'\0'};
    uuid_unparse_lower(uuid, request_id);

    return std::string(request_id);
}

void push_audio(const piat::common::RequestContext &request_context,
                const std::string &filename,
                const std::string &audio_data,
                const std::string &group_name,
                int sample_rate) {
    LOG(INFO) << request_context.get_request_id_for_log()
              << "Push audio to Dataset";

    std::string base64_data;
    base::Base64Encode(audio_data, &base64_data);

    piat::common::adaptor::AudioData audio(filename, base64_data, group_name, sample_rate);

    piat::common::adaptor::AudioDataHandler::GetInstance()->push_data(audio);
}

} // namespace common
