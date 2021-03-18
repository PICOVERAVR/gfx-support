#pragma once

#include "vformat.hpp"

#include <vector>

namespace ter {

// constants to adjust simplex noise frequency
constexpr static float xfscale = 0.05;
constexpr static float zfscale = 0.05;

class terrain {
public:
    // uint8_t specifies enum type
    enum features : uint8_t {
        normal = 0x1,
        uv = 0x2,
        tangent = 0x4,
    };

    terrain() {}
    terrain(unsigned int nwidth, unsigned int nheight, float xlim, float zlim, const uint8_t f);

    // regenerates terrain
    void regen(unsigned int nwidth, unsigned int nheight, float xlim, float zlim, const uint8_t f);

    // return height from (x, z) coordinates
    float getHeight(const float x, const float z);

    std::vector<vformat::vertex> verts;
    std::vector<uint32_t> indices;
};

}