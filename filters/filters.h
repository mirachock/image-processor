#pragma once

#include "transforms.h"
#include "../filter.h"

#include <memory>
#include <string_view>

namespace pixel_filter {
class PixelFilter : public virtual filter::Filter {
public:
    explicit PixelFilter(const pixel_transform::PixelTransform& pixel_transform);
    image::Image PixelFilterTransform(image::Image&& img) const;

private:
    pixel_transform::PixelTransform pixel_transform_;
};
}  // namespace pixel_filter

namespace matrix_filter {
class MatrixFilter : public virtual filter::Filter {
public:
    explicit MatrixFilter(const matrix_transform::MatrixTransform& matrix);
    image::Image MatrixFilterTransform(image::Image&& image) const;

private:
    matrix_transform::MatrixTransform matrix_;
};
}  // namespace matrix_filter

namespace crop {
class CropFilter : public filter::Filter {
public:
    explicit CropFilter(const std::vector<std::string_view>& params);
    image::Image Apply(image::Image&& image) const override;

private:
    size_t width_ = 0;
    size_t height_ = 0;
};
std::string_view Info();
void Check(const std::vector<std::string_view>& params);
}  // namespace crop

namespace gs {
class GsFilter : public pixel_filter::PixelFilter {
public:
    explicit GsFilter(const std::vector<std::string_view>& params);
    image::Image Apply(image::Image&& image) const override;
};
std::string_view Info();
void Check(const std::vector<std::string_view>& params);
}  // namespace gs

namespace neg {
class NegFilter : public pixel_filter::PixelFilter {
public:
    explicit NegFilter(const std::vector<std::string_view>& params);
    image::Image Apply(image::Image&& image) const override;
};
std::string_view Info();
void Check(const std::vector<std::string_view>& params);
}  // namespace neg

namespace sharp {
class SharpFilter : public matrix_filter::MatrixFilter {
public:
    explicit SharpFilter(const std::vector<std::string_view>& params);
    image::Image Apply(image::Image&& image) const override;
};
std::string_view Info();
void Check(const std::vector<std::string_view>& params);
}  // namespace sharp

namespace edge {
class EdgeFilter : public gs::GsFilter, public matrix_filter::MatrixFilter {
public:
    explicit EdgeFilter(const std::vector<std::string_view>& params);
    image::Image Apply(image::Image&& image) const override;

private:
    float threshold_ = 0;
};
std::string_view Info();
void Check(const std::vector<std::string_view>& params);
}  // namespace edge

namespace blur {
class BlurFilter : public filter::Filter {
public:
    explicit BlurFilter(const std::vector<std::string_view>& params);
    image::Image Apply(image::Image&& image) const override;

private:
    float sigma_ = 0;
};
std::string_view Info();
void Check(const std::vector<std::string_view>& params);
}  // namespace blur

namespace twirl {
namespace {
constexpr std::pair<size_t, size_t> DefaultCenter = {150, 150};
constexpr float DefaultRadius = 300;
constexpr float DefaultAngle = 3.14;
}  // namespace

class TwirlFilter : public filter::Filter {
public:
    explicit TwirlFilter(const std::vector<std::string_view>& params);
    image::Image Apply(image::Image&& image) const override;

private:
    std::pair<size_t, size_t> center_ = DefaultCenter;
    float radius_ = DefaultRadius;
    float angle_ = DefaultAngle;
};
std::string_view Info();
void Check(const std::vector<std::string_view>& params);
}  // namespace twirl