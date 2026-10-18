#include "filters.h"

#include "../errors.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <iostream>

namespace {
template <typename T>
void Read(std::string_view param, T& value) {
    std::from_chars_result res = std::from_chars(param.begin(), param.end(), value);
    if (res.ptr != param.end() || res.ec != std::errc{}) {
        throw error::ArgsError("Wrong format.");
    }
}
}  // namespace

namespace pixel_filter {
PixelFilter::PixelFilter(const pixel_transform::PixelTransform& pixel_transform) : pixel_transform_(pixel_transform) {
}

image::Image PixelFilter::PixelFilterTransform(image::Image&& image) const {
    return pixel_transform::MakePixelTransform(std::move(image), pixel_transform_);
}
}  // namespace pixel_filter

namespace matrix_filter {
MatrixFilter::MatrixFilter(const matrix_transform::MatrixTransform& matrix) : matrix_(matrix) {
}

image::Image MatrixFilter::MatrixFilterTransform(image::Image&& image) const {
    return matrix_transform::MakeMatrixTransform(std::move(image), matrix_);
}
}  // namespace matrix_filter

namespace crop {
CropFilter::CropFilter(const std::vector<std::string_view>& params) {
    Read(params[0], width_);
    Read(params[1], height_);
}

image::Image CropFilter::Apply(image::Image&& image) const {
    size_t new_width = std::min(width_, image.Width());
    size_t new_height = std::min(height_, image.Height());
    image::Image new_img(new_width, new_height);
    for (size_t y = 0; y < new_height; ++y) {
        for (size_t x = 0; x < new_width; ++x) {
            new_img.At(x, y) = image.At(x, y);
        }
    }
    return new_img;
}

std::string_view Info() {
    return "Crop (-crop width height)\n"
           "\tCrops the image to the specified width and height. The top-left part of the image is used. If the \n"
           "\trequested width or height exceeds the dimensions of the original image, the available part of the \n"
           "\timage is returned.\n";
}

void Check(const std::vector<std::string_view>& params) {
    if (params.size() != 2) {
        throw error::CropFilterError("Incorrect number of parameters.");
    }
    size_t width = 0;
    size_t height = 0;
    Read(params[0], width);
    Read(params[1], height);
    if (width == 0 || height == 0) {
        throw error::CropFilterError("The width/height must be greater than 0.");
    }
}
}  // namespace crop

namespace gs {
namespace {
constexpr pixel_transform::PixelTransform PIXEL({0.0f, 0.299f, 0.587f, 0.114f}, {0.0f, 0.299f, 0.587f, 0.114f},
                                                {0.0f, 0.299f, 0.587f, 0.114f});
}

GsFilter::GsFilter(const std::vector<std::string_view>&) : PixelFilter(PIXEL) {
}

image::Image GsFilter::Apply(image::Image&& image) const {
    return PixelFilterTransform(std::move(image));
}

std::string_view Info() {
    return "Grayscale (-gs)\n";
}

void Check(const std::vector<std::string_view>& params) {
    if (!params.empty()) {
        throw error::GsFilterError("Incorrect number of parameters.");
    }
}
}  // namespace gs

namespace neg {
namespace {
constexpr pixel_transform::PixelTransform PIXEL({1.0f, -1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, -1.0f, 0.0f},
                                                {1.0f, 0.0f, 0.0f, -1.0f});
}

NegFilter::NegFilter(const std::vector<std::string_view>&) : PixelFilter(PIXEL) {
}

image::Image NegFilter::Apply(image::Image&& image) const {
    return PixelFilterTransform(std::move(image));
}

std::string_view Info() {
    return "Negative (-neg)\n";
}

void Check(const std::vector<std::string_view>& params) {
    if (!params.empty()) {
        throw error::NegFilterError("Incorrect number of parameters.");
    }
}
}  // namespace neg

namespace sharp {
namespace {
const matrix_transform::MatrixTransform MATRIX(3, {0, -1, 0, -1, 5, -1, 0, -1, 0});
}

SharpFilter::SharpFilter(const std::vector<std::string_view>&) : MatrixFilter(MATRIX) {
}

image::Image SharpFilter::Apply(image::Image&& image) const {
    return MatrixFilterTransform(std::move(image));
}

std::string_view Info() {
    return "Sharpening (-sharp)\n";
}

void Check(const std::vector<std::string_view>& params) {
    if (!params.empty()) {
        throw error::SharpFilterError("Incorrect number of parameters.");
    }
}
}  // namespace sharp

namespace edge {
namespace {
const matrix_transform::MatrixTransform EDGE_MATRIX(3, {0.0f, -1.0f, 0.0f, -1.0f, 4.0f, -1.0f, 0.0f, -1.0f, 0.0f});
}

EdgeFilter::EdgeFilter(const std::vector<std::string_view>& params) : GsFilter({}), MatrixFilter(EDGE_MATRIX) {
    Read(params[0], threshold_);
}

image::Image EdgeFilter::Apply(image::Image&& image) const {
    image = GsFilter::Apply(std::move(image));
    image = MatrixFilterTransform(std::move(image));
    for (size_t x = 0; x < image.Width(); ++x) {
        for (size_t y = 0; y < image.Height(); ++y) {
            image.At(x, y) = color::Color(1.0f, 1.0f, 1.0f) * (image.At(x, y).red > threshold_);
        }
    }
    return image;
}

std::string_view Info() {
    return "Edge Detection (-edge threshold)\n";
}

void Check(const std::vector<std::string_view>& params) {
    if (params.size() != 1) {
        throw error::EdgeFilterError("Incorrect number of parameters.");
    }
    float threshold = 0;
    Read(params[0], threshold);
    if (!std::isfinite(threshold) || threshold < 0.0f || threshold > 1.0f) {
        throw error::EdgeFilterError("The threshold must be a float number from 0 to 1.");
    }
}
}  // namespace edge

namespace blur {
namespace {
constexpr float PI = std::numbers::pi;
}

BlurFilter::BlurFilter(const std::vector<std::string_view>& params) {
    Read(params[0], sigma_);
}

image::Image BlurFilter::Apply(image::Image&& image) const {
    const size_t k = static_cast<size_t>(3 * sigma_) + 1;
    image::Image image1(image.Width(), image.Height());
    for (size_t x = 0; x < image.Width(); ++x) {
        for (size_t y = 0; y < image.Height(); ++y) {
            for (size_t i = (x >= k ? x - k : 0); i <= std::min(image.Width() - 1, x + k); ++i) {
                float dist = static_cast<float>((x - i) * (x - i));
                float coef = (std::exp(-dist / (2 * sigma_ * sigma_)) / (sqrtf(2 * PI) * sigma_));
                image1.At(x, y) = image1.At(x, y) + image.At(i, y) * coef;
            }
        }
    }
    for (size_t x = 0; x < image.Width(); ++x) {
        for (size_t y = 0; y < image.Height(); ++y) {
            image.At(x, y) = color::Color(0, 0, 0);
            for (size_t j = (y >= k ? y - k : 0); j <= std::min(image.Height() - 1, y + k); ++j) {
                float dist = static_cast<float>((y - j) * (y - j));
                float coef = (std::exp(-dist / (2 * sigma_ * sigma_)) / (sqrtf(2 * PI) * sigma_));
                image.At(x, y) = image.At(x, y) + image1.At(x, j) * coef;
            }
        }
    }
    for (size_t x = 0; x < image.Width(); ++x) {
        for (size_t y = 0; y < image.Height(); ++y) {
            image.At(x, y).Normalize();
        }
    }
    return image;
}

std::string_view Info() {
    return "Gaussian Blur (-blur sigma)\n"
           "\tImage blurring method.\n";
}

void Check(const std::vector<std::string_view>& params) {
    if (params.size() != 1) {
        throw error::GaussianFilterError("Incorrect number of parameters.");
    }
    float sigma = 0;
    Read(params[0], sigma);
    if (!std::isfinite(sigma) || sigma <= 0.0f) {
        throw error::GaussianFilterError("The sigma must be a float number greater than 0.");
    }
}
}  // namespace blur

namespace twirl {

TwirlFilter::TwirlFilter(const std::vector<std::string_view>& params) {
    Read(params[0].substr(1), center_.first);
    Read(params[1].substr(0, params[1].size() - 1), center_.second);
    Read(params[2], radius_);
    Read(params[3], angle_);
}

image::Image TwirlFilter::Apply(image::Image&& image) const {
    image::Image new_image(image.Width(), image.Height());
    for (size_t x = 0; x < image.Width(); ++x) {
        for (size_t y = 0; y < image.Height(); ++y) {
            std::pair<float, float> vect = {static_cast<float>(x) - static_cast<float>(center_.first),
                                            static_cast<float>(y) - static_cast<float>(center_.second)};
            float dist2 = vect.first * vect.first + vect.second * vect.second;
            if (dist2 >= radius_ * radius_) {
                new_image.At(x, y) = image.At(x, y);
                continue;
            }
            float dist = sqrtf(dist2);

            float phi = angle_ * (1.0f - dist / radius_) * (1.0f - dist / radius_);

            float i_float =
                roundf(static_cast<float>(center_.first) + vect.first * cosf(phi) - vect.second * sinf(phi));
            i_float = std::min(static_cast<float>(image.Width()) - 1.0f, std::max(0.0f, i_float));

            float j_float =
                roundf(static_cast<float>(center_.second) + vect.first * sinf(phi) + vect.second * cosf(phi));
            j_float = std::min(static_cast<float>(image.Height()) - 1.0f, std::max(0.0f, j_float));

            size_t i = static_cast<size_t>(i_float);
            size_t j = static_cast<size_t>(j_float);

            new_image.At(x, y) = image.At(i, j);
        }
    }
    return new_image;
}

std::string_view Info() {
    return "Twirl Distortion (-twirl center radius angle)\n"
           "\tRotates and twists the image around a specified central point, creating the illusion of a funnel or \n"
           "\twhirlpool. The center should look like [x y]. Example of a filter call: -twirl [150 150] 300.00 3.14\n";
}

void Check(const std::vector<std::string_view>& params) {
    if (params.size() != 4) {
        throw error::TwirlFilterError("Incorrect number of parameters.");
    }
    if (params[0].size() < 2 || params[1].size() < 2 || params[0][0] != '[' || params[1].back() != ']') {
        throw error::TwirlFilterError("Incorrect center entry.");
    }
    std::pair<size_t, size_t> canter;
    float radius = 0;
    float angle = 0;
    Read(params[0].substr(1), canter.first);
    Read(params[1].substr(0, params[1].size() - 1), canter.second);
    Read(params[2], radius);
    Read(params[3], angle);
    if (!std::isfinite(radius) || !std::isfinite(angle) || radius < 0.0f) {
        throw error::TwirlFilterError("The radius must be a float number greater or equal than 0.");
    }
}
}  // namespace twirl