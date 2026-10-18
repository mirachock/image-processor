#include "image.h"

#include "errors.h"

namespace color {

Color::Color(float red, float green, float blue) : red(red), green(green), blue(blue) {
}

Color::Color() = default;

Color Color::operator+(const Color& color) const {
    return {red + color.red, green + color.green, blue + color.blue};
}

Color Color::operator*(float x) const {
    return {red * x, green * x, blue * x};
}

void Color::Normalize() {
    red = std::min(1.0f, std::max(0.0f, red));
    green = std::min(1.0f, std::max(0.0f, green));
    blue = std::min(1.0f, std::max(0.0f, blue));
}

}  // namespace color

namespace image {

Image::Image(size_t width, size_t height) : width_(width), height_(height), pixels_(width * height) {
}

size_t Image::Width() const {
    return width_;
}

size_t Image::Height() const {
    return height_;
}

color::Color& Image::At(size_t x, size_t y) {
    if (x >= width_ || y >= height_) {
        throw error::Defect("Invalid Image access (out of bounds).");
    }
    return pixels_[y * width_ + x];
}

const color::Color& Image::At(size_t x, size_t y) const {
    if (x >= width_ || y >= height_) {
        throw error::Defect("Invalid Image access (out of bounds).");
    }
    return pixels_[y * width_ + x];
}

}  // namespace image
