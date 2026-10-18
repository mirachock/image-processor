#pragma once

#include "errors.h"

#include <cstdint>
#include <fstream>
#include <limits>

namespace file {

namespace {
constexpr int BitsPerByte = std::numeric_limits<unsigned char>::digits;
}

template <typename T>
T ReadTo(std::istream& in) {
    unsigned char bytes[sizeof(T)];
    if (!in.read(reinterpret_cast<char*>(bytes), sizeof(T))) {
        throw error::InputFileError("The reading was interrupted.");
    }
    uint64_t value = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        value |= static_cast<uint64_t>(bytes[i]) << (BitsPerByte * i);
    }
    return static_cast<T>(value);
}

template <typename T>
void WriteTo(std::ostream& out, T val) {
    unsigned char bytes[sizeof(T)];
    uint64_t value = static_cast<uint64_t>(val);
    for (size_t i = 0; i < sizeof(T); ++i) {
        bytes[i] = static_cast<unsigned char>(value >> (BitsPerByte * i));
    }
    if (!out.write(reinterpret_cast<char*>(bytes), sizeof(T))) {
        throw error::OutputFileError("The writing was interrupted.");
    }
}

}  // namespace file
