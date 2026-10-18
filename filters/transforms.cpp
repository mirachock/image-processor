#include "transforms.h"

#include "../errors.h"

namespace matrix_transform {

MatrixTransform::MatrixTransform(size_t n, const std::vector<float>& matrix) : n(n), matrix(matrix) {
    if (n % 2 == 0 || matrix.size() != n * n) {
        throw error::Defect("Wrong Matrix size.");
    }
}

const float& MatrixTransform::At(size_t x, size_t y) const {
    if (y >= n || x >= n) {
        throw error::Defect("Invalid Matrix access (out of bounds).");
    }
    return matrix[y * n + x];
}

image::Image MakeMatrixTransform(image::Image&& image, const MatrixTransform& matrix) {
    image::Image new_image(image.Width(), image.Height());
    for (size_t x = 0; x < image.Width(); ++x) {
        for (size_t y = 0; y < image.Height(); ++y) {
            size_t mid = matrix.n / 2;
            for (size_t i = 0; i < matrix.n; ++i) {
                for (size_t j = 0; j < matrix.n; ++j) {
                    size_t ind_x = std::min(x + i >= mid ? x + i - mid : 0, image.Width() - 1);
                    size_t ind_y = std::min(y + j >= mid ? y + j - mid : 0, image.Height() - 1);
                    new_image.At(x, y) = new_image.At(x, y) + image.At(ind_x, ind_y) * matrix.At(i, j);
                }
            }
            new_image.At(x, y).Normalize();
        }
    }
    return new_image;
}

}  // namespace matrix_transform

namespace pixel_transform {

float ColorTransform::Apply(const color::Color& color) const {
    return bias + red * color.red + green * color.green + blue * color.blue;
}

color::Color PixelTransform::Apply(const color::Color& color) const {
    return color::Color(red.Apply(color), green.Apply(color), blue.Apply(color));
}

image::Image MakePixelTransform(image::Image&& image, const PixelTransform& colorvector) {
    for (size_t x = 0; x < image.Width(); ++x) {
        for (size_t y = 0; y < image.Height(); ++y) {
            image.At(x, y) = colorvector.Apply(image.At(x, y));
            image.At(x, y).Normalize();
        }
    }
    return image;
}

}  // namespace pixel_transform
