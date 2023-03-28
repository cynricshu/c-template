
#include <chrono>
#include <iostream>

using namespace std::chrono;

struct Test {
    explicit Test(const std::string &trace_id) : trace_id_(trace_id) {};
    const std::string &trace_id_;
};

Test CreateTest() {
    // 栈上的对象，如果被当成引用传入别处，栈结束后，指向的内存就会变成未定义
    std::string t = "hello world";
    return Test(t);
}

int main(int argc, char **argv) {
    auto t = CreateTest();
    std::cout << t.trace_id_ << std::endl;

    auto now_tp = system_clock::now();
    time_t now_time_t = system_clock::to_time_t(now_tp);
    tm *local_time = std::localtime(&now_time_t);
    local_time->tm_hour = 0;
    local_time->tm_min = 0;
    local_time->tm_sec = 0;
    time_t local_zero_time = timelocal(local_time);
    std::cout << asctime(local_time) << (local_time);
    time_t gmt_zero_time = timegm(local_time);

    long ts = local_zero_time;

    long current_ts = duration_cast<milliseconds>(now_tp.time_since_epoch()).count();
}