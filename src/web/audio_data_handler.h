// Copyright 2019 Baidu Inc. All Rights Reserved.

#pragma once

#include <assert.h>
#include <base/logging.h>
#include <curl/curl.h>

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <string>
#include <utility>

#include "common/request_context.h"

template<typename T> struct DefaultSingletonTraits;

namespace web {

void push_audio(const piat::common::RequestContext &request_context,
                const std::string &filename,
                const std::string &audio_data,
                const std::string &group_name,
                int sample_rate = 8000);

struct AudioData {
    AudioData(std::string filename,
              std::string content,
              std::string group_name,
              int sample_rate = 8000) :
        filename(std::move(filename)),
        content(std::move(content)),
        group_name(std::move(group_name)),
        sample_rate(sample_rate) {};

    std::string filename;
    std::string content;
    std::string group_name;
    int sample_rate;
    std::string source_type = "LANGUAGE_MODEL";
    int retry_time = 0; // do not set this value
};

class AudioDataQueue {
public:
    AudioDataQueue() : _mutex(), _not_empty(), _queue() {}
    ~AudioDataQueue() = default;
    AudioDataQueue(const AudioData &queue) = delete;
    AudioDataQueue &operator=(const AudioDataQueue &queue) = delete;

    void put(const AudioData &x) {
        std::lock_guard<std::mutex> guard(_mutex);
        _queue.push_back(x);
        _not_empty.notify_one();
    }

    AudioData &take() {
        std::unique_lock<std::mutex> lock(_mutex);
        _not_empty.wait(lock, [this] { return !this->_queue.empty(); });
        assert(!_queue.empty());
        return _queue.front();
    }

    void pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_queue.empty()) {
            _queue.pop_front();
        }
    }

    size_t size() const {
        std::lock_guard<std::mutex> guard(_mutex);
        return _queue.size();
    }

private:
    mutable std::mutex _mutex;
    std::condition_variable _not_empty;
    std::deque<AudioData> _queue;
};

class AudioDataHandler {
public:
    static AudioDataHandler *GetInstance();

    int init();
    void push_data(AudioData &task);

private:
    friend struct DefaultSingletonTraits<AudioDataHandler>;

    AudioDataHandler() : _running(false) {}
    ~AudioDataHandler();

    void start_worker();
    void run_task();
    int handle_data(AudioData &task);
    int post_msg(const std::string &url, const std::string &data,
                      std::string *response_string);

    bool _running;
    std::thread _thread;
    AudioDataQueue _task_queue;

    std::string _endpoint;
    std::string _create_audio_path;
};

} // namespace web