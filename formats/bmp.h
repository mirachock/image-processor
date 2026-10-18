#pragma once

#include "../errors.h"
#include "../image.h"

#include <cstdint>

namespace bit_map_header {

namespace {
constexpr uint16_t BfType = 0x4D42;  // "BM"
constexpr uint16_t BfReserved = 0;
constexpr uint32_t BfOffBits = 14;
constexpr uint32_t BiSize = 40;
constexpr uint16_t BiPlanes = 1;
constexpr uint16_t BiBitCount = 24;
constexpr uint32_t BiCompression = 0;
constexpr uint32_t BiClrUsed = 0;
}  // namespace

class BitMapFileHeader {
public:
    void Read(std::istream& in);
    void Write(std::ofstream& out) const;
    void Normalize() const;
    uint32_t& Size();
    uint32_t OffBits() const;

private:
    uint16_t bf_type_ = BfType;
    uint32_t bf_size_ = 0;
    uint16_t bf_reserved1_ = BfReserved;
    uint16_t bf_reserved2_ = BfReserved;
    uint32_t bf_off_bits_ = BfOffBits + BiSize;
};

class BitMapInfoHeader {
public:
    void Read(std::istream& in);
    void Write(std::ofstream& out) const;
    void Normalize();
    int32_t Width() const;
    int32_t& Width();
    int32_t Height() const;
    int32_t& Height();
    uint32_t SizeImage() const;
    uint32_t& SizeImage();

private:
    uint32_t bi_size_ = BiSize;
    int32_t bi_width_ = 0;
    int32_t bi_height_ = 0;
    uint16_t bi_planes_ = BiPlanes;
    uint16_t bi_bit_count_ = BiBitCount;
    uint32_t bi_compression_ = BiCompression;
    uint32_t bi_size_image_ = 0;
    int32_t bi_x_pels_per_meter_ = 0;
    int32_t bi_y_pels_per_meter_ = 0;
    uint32_t bi_clr_used_ = BiClrUsed;
    uint32_t bi_clr_important_ = 0;
};

struct BitMapHeaders {
    BitMapFileHeader bmh_file;
    BitMapInfoHeader bmh_info;
    void Read(std::istream& in);
    void Write(std::ofstream& out) const;
    void Normalize();
    void Fit(const image::Image& image);
};

}  // namespace bit_map_header

namespace bmp {

image::Image Read(std::string_view path);

void Write(std::string_view path, const image::Image& img);

}  // namespace bmp
