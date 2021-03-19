#pragma once

#include <string>
#include <cstdint>

namespace iload {

class iloader {
public:
    iloader();
	iloader(std::string_view path, bool flip);

    void load(std::string_view path, bool flip);

    ~iloader();

    int width;
    int height;
    uint8_t* data;
};

}