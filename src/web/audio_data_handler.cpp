// Copyright 2019 Baidu Inc. All Rights Reserved.

#include "audio_data_handler.h"

#include <gflags/gflags.h>
#include <base/base64.h>
#include <base/memory/singleton.h>
#include <json/json.h>
#include <sstream>

DEFINE_string(dataset_endpoint,
"10.136.172.11:8995", "the endpoint of audioDataset Service");
DEFINE_string(create_audio_path,
"/api/audioDataset/v1/audio/create","the restapi path of create audio");
DEFINE_string(group_name_format,
"streaming_server_{app}_{username}_{product_id}","the format of group name in audiodataset");
DEFINE_int32(max_try_time, 1, "max try time when send data");


namespace web {

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

static size_t write_callback(void *ptr, size_t size, size_t nmemb,
                             std::string *data) {
    data->append((char *) ptr, size * nmemb);
    return size * nmemb;
}

AudioDataHandler::~AudioDataHandler() {
    _running = false;
    if (_thread.joinable()) {
        _thread.join();
    }
    curl_global_cleanup();
}

void AudioDataHandler::push_data(AudioData &task) {
    _task_queue.put(task);
}

void AudioDataHandler::start_worker() {
    _running = true;
    _thread = std::thread(&AudioDataHandler::run_task, this);
}

void AudioDataHandler::run_task() {
    int ret = 1;
    while (_running) {
        size_t size = _task_queue.size();
        if (size == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        AudioData &task = _task_queue.take();
        ret = handle_data(task);
        if (0 != ret) {
            LOG(ERROR) << "Fail to push audio! retry time: "
                       << task.retry_time;
            if (++task.retry_time < FLAGS_max_try_time) {
                continue;
            }
        }
        _task_queue.pop();
    }
}

int AudioDataHandler::handle_data(AudioData &task) {
    /*Json Msg to post*/
    Json::Value value(Json::objectValue);
    value["sourceType"] = task.source_type;
    value["file"] = task.content;
    value["audioName"] = task.filename;
    value["audioGroupName"] = task.group_name;
    value["sampleRate"] = task.sample_rate;
    value["trackNum"] = 1;
    value["needCreateGroupByNameIfNotExist"] = true;
    value["needRecognize"] = false;
    //value["modelType"] = model_type.c_str();

    Json::FastWriter writer;
    std::string data = writer.write(value);

    /*post data & get response*/
    const std::string url = _endpoint + _create_audio_path;

    std::string response_string;
    if (0 != post_msg(url, data, &response_string)) {
        LOG(ERROR) << "failed to post_msg";
        return -1;
    }
    return 0;
}

int AudioDataHandler::post_msg(const std::string &url,
                                    const std::string &data,
                                    std::string *response_string) {
    CURLcode ret;
    CURL *curl = curl_easy_init();
    if (curl) {
        struct curl_slist *hs = NULL;
        hs = curl_slist_append(hs, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_string);

        ret = curl_easy_perform(curl);
        curl_slist_free_all(hs);
        curl_easy_cleanup(curl);
    } else {
        LOG(ERROR) << "Failed: curl_easy_init()";
        return 1;
    }

    if (CURLE_OK != ret) {
        LOG(ERROR) << "http status: " << ret
                   << " url: " << url << " response_string: " << response_string;
        return 1;
    }

    return 0;
}

int AudioDataHandler::init() {
    LOG(NOTICE) << "init AudioDataHandler";

    _endpoint = FLAGS_dataset_endpoint;
    _create_audio_path = FLAGS_create_audio_path;
    curl_global_init(CURL_GLOBAL_ALL);

    LOG(NOTICE) << "_endpoint=" << _endpoint << ", _create_audio_path=" << _create_audio_path;
    start_worker();
    return 0;
}

AudioDataHandler *AudioDataHandler::GetInstance() {
    return Singleton<AudioDataHandler>::get();
}

} // namespace web
