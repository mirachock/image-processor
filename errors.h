#pragma once

#include <stdexcept>
#include <string_view>

namespace error {

class Defect : public std::logic_error {
public:
    using std::logic_error::logic_error;
    virtual std::string_view Name() const {
        return "Defect";
    }
};

class Error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
    virtual std::string_view Name() const {
        return "Error";
    }
};

class FileError : public Error {
public:
    using Error::Error;
    std::string_view Name() const override {
        return "FileError";
    }
};

class InputFileError : public FileError {
public:
    using FileError::FileError;
    std::string_view Name() const override {
        return "InputFileError";
    }
};

class OutputFileError : public FileError {
public:
    using FileError::FileError;
    std::string_view Name() const override {
        return "OutputFileError";
    }
};

class OutputFormatError : public Error {
public:
    using Error::Error;
    std::string_view Name() const override {
        return "OutputFormatError";
    }
};

class BitMapHeaderError : public Error {
public:
    using Error::Error;
    std::string_view Name() const override {
        return "BitMapHeaderError";
    }
};

class BitMapFileHeaderError : public BitMapHeaderError {
public:
    using BitMapHeaderError::BitMapHeaderError;
    std::string_view Name() const override {
        return "BitMapFileHeaderError";
    }
};

class BitMapInfoHeaderError : public BitMapHeaderError {
public:
    using BitMapHeaderError::BitMapHeaderError;
    std::string_view Name() const override {
        return "BitMapInfoHeaderError";
    }
};

class ArgsError : public Error {
public:
    using Error::Error;
    std::string_view Name() const override {
        return "ArgsError";
    }
};

class FilterError : public ArgsError {
public:
    using ArgsError::ArgsError;
    std::string_view Name() const override {
        return "FilterError";
    }
};

class CropFilterError : public FilterError {
public:
    using FilterError::FilterError;
    std::string_view Name() const override {
        return "CropFilterError";
    }
};

class SharpFilterError : public FilterError {
public:
    using FilterError::FilterError;
    std::string_view Name() const override {
        return "SharpFilterError";
    }
};

class EdgeFilterError : public FilterError {
public:
    using FilterError::FilterError;
    std::string_view Name() const override {
        return "EdgeFilterError";
    }
};

class GsFilterError : public FilterError {
public:
    using FilterError::FilterError;
    std::string_view Name() const override {
        return "GsFilterError";
    }
};

class NegFilterError : public FilterError {
public:
    using FilterError::FilterError;
    std::string_view Name() const override {
        return "NegFilterError";
    }
};

class GaussianFilterError : public FilterError {
public:
    using FilterError::FilterError;
    std::string_view Name() const override {
        return "GaussianFilterError";
    }
};

class TwirlFilterError : public FilterError {
public:
    using FilterError::FilterError;
    std::string_view Name() const override {
        return "TwirlFilterError";
    }
};

}  // namespace error
