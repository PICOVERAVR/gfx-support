#pragma once

#include <string>
#include <cstdint>
#include <thread>

namespace iload {

class iloader {
public:
    iloader();
	iloader(std::string_view path, bool flip);

    // blocking load
    void load();

    // non-blocking threaded load
    void dispatch();
    void join();

    ~iloader();

    std::string path;
    bool flip;

    std::thread t;

    int width;
    int height;
    uint8_t* data;
};

}
