#pragma once

#include "../image.h"

namespace matrix_transform {

struct MatrixTransform {
    const size_t n;
    const std::vector<float> matrix;
    MatrixTransform(size_t n, const std::vector<float>& matrix);
    const float& At(size_t x, size_t y) const;
};

image::Image MakeMatrixTransform(image::Image&& img, const MatrixTransform& matrix);

}  // namespace matrix_transform

namespace pixel_transform {

struct ColorTransform {
    const float bias;
    const float red;
    const float green;
    const float blue;
    float Apply(const color::Color& color) const;
};

struct PixelTransform {
    ColorTransform red;
    ColorTransform green;
    ColorTransform blue;
    color::Color Apply(const color::Color& color) const;
};

image::Image MakePixelTransform(image::Image&& img, const PixelTransform& colorvector);

}  // namespace pixel_transform
