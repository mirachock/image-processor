#pragma once

#include <vector>

namespace color {

struct Color {
    float red = 0;
    float green = 0;
    float blue = 0;
    Color(float red, float green, float blue);
    Color();
    Color operator+(const Color& color) const;
    Color operator*(float x) const;
    void Normalize();
};

}  // namespace color

namespace image {

class Image {
public:
    Image(size_t width, size_t height);
    size_t Width() const;
    size_t Height() const;
    color::Color& At(size_t x, size_t y);
    const color::Color& At(size_t x, size_t y) const;

private:
    size_t width_ = 0;
    size_t height_ = 0;
    std::vector<color::Color> pixels_;
};

}  // namespace image
