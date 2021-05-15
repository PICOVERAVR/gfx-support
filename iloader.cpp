#include "iloader.hpp"

#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
// take out decoders we don't use to save space
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#include "stb_image.h"

namespace iload {

iloader::iloader(std::string_view npath, bool nflip) : path(npath), flip(nflip) { }

void iloader::load() {
    // if the image format considers the origin to be the top left (png), then flip.
    stbi_set_flip_vertically_on_load_thread(flip);

    int chans;
    data = stbi_load(path.data(), &width, &height, &chans, STBI_rgb_alpha);
    if (!data) {
        const std::string errstr = std::string("cannot load texture ") + path.data() + "!";
        throw std::runtime_error(errstr);
    }
}

void iloader::dispatch() {
    t = std::thread([&]() -> void { load(); });
}

void iloader::join() {
    if (!t.joinable()) {
        throw std::runtime_error("cannot join non-joinable thread!");
    }

    t.join();
}

iloader::~iloader() {
    if (data) {
        stbi_image_free(data);
    }
}

}
