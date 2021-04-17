#include "iloader.hpp"

#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
// take out decoders we don't use to save space
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#include "stb_image.h"

namespace iload {

iloader::iloader() : width(0), height(0), data(nullptr) {}

iloader::iloader(std::string_view path, bool flip) {
    load(path, flip);
}

void iloader::load(std::string_view path, bool flip) {
    // if the image format considers the origin to be the top left (png), then flip.
    stbi_set_flip_vertically_on_load_thread(flip);

    int chans;
    data = stbi_load(path.data(), &width, &height, &chans, STBI_rgb_alpha);
    if (!data) {
        throw std::runtime_error("cannot load texture!");
    }
}

iloader::~iloader() {
    if (data) {
        stbi_image_free(data);
    }
}

}
