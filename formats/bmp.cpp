#include "bmp.h"

#include "../file.h"

#include <cmath>
#include <fstream>
#include <limits>

namespace bit_map_header {

void BitMapFileHeader::Read(std::istream& in) {
    bf_type_ = file::ReadTo<uint16_t>(in);
    bf_size_ = file::ReadTo<uint32_t>(in);
    bf_reserved1_ = file::ReadTo<uint16_t>(in);
    bf_reserved2_ = file::ReadTo<uint16_t>(in);
    bf_off_bits_ = file::ReadTo<uint32_t>(in);
}

void BitMapFileHeader::Write(std::ofstream& out) const {
    file::WriteTo(out, bf_type_);
    file::WriteTo(out, bf_size_);
    file::WriteTo(out, bf_reserved1_);
    file::WriteTo(out, bf_reserved2_);
    file::WriteTo(out, bf_off_bits_);
}

void BitMapFileHeader::Normalize() const {
    if (bf_type_ != BfType) {
        throw error::BitMapFileHeaderError("Incorrect bfType");
    }
    if (bf_reserved1_ != BfReserved || bf_reserved2_ != BfReserved) {
        throw error::BitMapFileHeaderError("Incorrect bfReserved");
    }
    if (bf_off_bits_ < BfOffBits + BiSize) {
        throw error::BitMapFileHeaderError("Incorrect bfOffBits");
    }
}

uint32_t& BitMapFileHeader::Size() {
    return bf_size_;
}

uint32_t BitMapFileHeader::OffBits() const {
    return bf_off_bits_;
}

void BitMapInfoHeader::Read(std::istream& in) {
    bi_size_ = file::ReadTo<uint32_t>(in);
    bi_width_ = file::ReadTo<int32_t>(in);
    bi_height_ = file::ReadTo<int32_t>(in);
    bi_planes_ = file::ReadTo<uint16_t>(in);
    bi_bit_count_ = file::ReadTo<uint16_t>(in);
    bi_compression_ = file::ReadTo<uint32_t>(in);
    bi_size_image_ = file::ReadTo<uint32_t>(in);
    bi_x_pels_per_meter_ = file::ReadTo<int32_t>(in);
    bi_y_pels_per_meter_ = file::ReadTo<int32_t>(in);
    bi_clr_used_ = file::ReadTo<uint32_t>(in);
    bi_clr_important_ = file::ReadTo<uint32_t>(in);
}

void BitMapInfoHeader::Write(std::ofstream& out) const {
    file::WriteTo(out, bi_size_);
    file::WriteTo(out, bi_width_);
    file::WriteTo(out, bi_height_);
    file::WriteTo(out, bi_planes_);
    file::WriteTo(out, bi_bit_count_);
    file::WriteTo(out, bi_compression_);
    file::WriteTo(out, bi_size_image_);
    file::WriteTo(out, bi_x_pels_per_meter_);
    file::WriteTo(out, bi_y_pels_per_meter_);
    file::WriteTo(out, bi_clr_used_);
    file::WriteTo(out, bi_clr_important_);
}

void BitMapInfoHeader::Normalize() {
    if (bi_size_ != BiSize) {
        throw error::BitMapInfoHeaderError("Incorrect biSize");
    }
    if (bi_width_ <= 0) {
        throw error::BitMapInfoHeaderError("Incorrect biWidth");
    }
    if (bi_height_ == 0) {
        throw error::BitMapInfoHeaderError("Incorrect biHeight");
    }
    if (bi_planes_ != BiPlanes) {
        throw error::BitMapInfoHeaderError("Incorrect biPlanes");
    }
    if (bi_bit_count_ != BiBitCount) {
        throw error::BitMapInfoHeaderError("Incorrect biBitCount");
    }
    if (bi_compression_ != BiCompression) {
        throw error::BitMapInfoHeaderError("Incorrect biCompression");
    }
    if (bi_clr_used_ != BiClrUsed) {
        throw error::BitMapInfoHeaderError("Incorrect biClrUsed");
    }

    size_t width = bi_width_;
    size_t height = bi_height_;
    bool height_is_negative = bi_height_ < 0;
    if (height_is_negative) {
        height = -static_cast<int64_t>(bi_height_);
    }
    size_t true_bi_size_image = (3 * width + width % 4) * height;
    if (bi_size_image_ == 0) {
        if (true_bi_size_image > std::numeric_limits<uint32_t>::max()) {
            throw error::BitMapInfoHeaderError("biSizeImage error. The image is too large.");
        }
        bi_size_image_ = true_bi_size_image;
    }
    if (bi_size_image_ != true_bi_size_image) {
        throw error::InputFileError("Incorrect biSizeImage.");
    }
}

int32_t BitMapInfoHeader::Width() const {
    return bi_width_;
}

int32_t& BitMapInfoHeader::Width() {
    return bi_width_;
}

int32_t BitMapInfoHeader::Height() const {
    return bi_height_;
}

int32_t& BitMapInfoHeader::Height() {
    return bi_height_;
}

uint32_t BitMapInfoHeader::SizeImage() const {
    return bi_size_image_;
}

uint32_t& BitMapInfoHeader::SizeImage() {
    return bi_size_image_;
}

void BitMapHeaders::Read(std::istream& in) {
    bmh_file.Read(in);
    bmh_info.Read(in);
}

void BitMapHeaders::Write(std::ofstream& out) const {
    bmh_file.Write(out);
    bmh_info.Write(out);
}

void BitMapHeaders::Normalize() {
    bmh_file.Normalize();
    bmh_info.Normalize();

    size_t true_bf_size = static_cast<size_t>(bmh_info.SizeImage()) + static_cast<size_t>(bmh_file.OffBits());

    if (true_bf_size > bmh_file.Size()) {
        throw error::BitMapHeaderError("Incorrect biSize.");
    }
}

void BitMapHeaders::Fit(const image::Image& image) {
    if (image.Width() > std::numeric_limits<int32_t>::max()) {
        throw error::OutputFormatError("The width is too large.");
    }
    bmh_info.Width() = static_cast<int32_t>(image.Width());
    if (image.Height() >= std::numeric_limits<int32_t>::max()) {
        throw error::OutputFormatError("The height is too large.");
    }
    bmh_info.Height() = static_cast<int32_t>(-static_cast<int64_t>(image.Height()));
    if ((3 * image.Width() + image.Width() % 4) * image.Height() > std::numeric_limits<uint32_t>::max()) {
        throw error::OutputFormatError("The biSizeImage is too large.");
    }
    bmh_info.SizeImage() = (3 * image.Width() + image.Width() % 4) * image.Height();
    if (bmh_file.OffBits() > std::numeric_limits<uint32_t>::max() - bmh_info.SizeImage()) {
        throw error::OutputFormatError("The biSize is too large.");
    }
    bmh_file.Size() = bmh_file.OffBits() + bmh_info.SizeImage();
}

}  // namespace bit_map_header

namespace bmp {

namespace {
constexpr float MaxColor = 255.0f;
}

image::Image Read(std::string_view path) {
    std::ifstream in((std::string(path)), std::ios::binary);
    if (!in) {
        throw error::InputFileError("The input-file did not open.");
    }

    bit_map_header::BitMapHeaders bmh;
    bmh.Read(in);
    bmh.Normalize();  // checking for bmp-errors

    in.seekg(0, std::ios::end);
    std::streampos file_size = in.tellg();
    if (file_size != bmh.bmh_file.Size()) {
        throw error::InputFileError("The input-file is corrupted.");
    }
    in.seekg(bmh.bmh_file.OffBits());

    size_t width = bmh.bmh_info.Width();
    size_t height = bmh.bmh_info.Height();
    bool height_is_negative = bmh.bmh_info.Height() < 0;
    if (height_is_negative) {
        height = -static_cast<int64_t>(bmh.bmh_info.Height());
    }

    image::Image image(width, height);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            float blue = static_cast<float>(file::ReadTo<uint8_t>(in)) / MaxColor;
            float green = static_cast<float>(file::ReadTo<uint8_t>(in)) / MaxColor;
            float red = static_cast<float>(file::ReadTo<uint8_t>(in)) / MaxColor;
            color::Color color(red, green, blue);
            if (height_is_negative) {
                image.At(x, y) = color;
            } else {
                image.At(x, height - 1 - y) = color;
            }
        }
        for (size_t x = 0; x < width % 4; ++x) {
            file::ReadTo<uint8_t>(in);
        }
    }

    in.close();
    if (!in) {
        throw error::InputFileError("An issue arose while closing the in-file");
    }

    return image;
}

void Write(std::string_view path, const image::Image& image) {
    std::ofstream out((std::string(path)), std::ios::binary);
    if (!out) {
        throw error::OutputFileError("The output did not open");
    }

    bit_map_header::BitMapHeaders bmh;
    bmh.Fit(image);  // checking for bmp-errors
    bmh.Write(out);

    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            const auto& [r, g, b] = image.At(x, y);
            file::WriteTo(out, static_cast<uint8_t>(round(b * MaxColor)));
            file::WriteTo(out, static_cast<uint8_t>(round(g * MaxColor)));
            file::WriteTo(out, static_cast<uint8_t>(round(r * MaxColor)));
        }
        for (size_t x = 0; x < image.Width() % 4; ++x) {
            file::WriteTo(out, static_cast<uint8_t>(0));
        }
    }

    out.close();
    if (!out) {
        throw error::OutputFileError("An issue arose while closing the out-file");
    }
}

}  // namespace bmp
