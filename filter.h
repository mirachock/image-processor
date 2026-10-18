#pragma once

#include "image.h"

namespace filter {

class Filter {
public:
    virtual ~Filter() = default;
    virtual image::Image Apply(image::Image&& img) const = 0;
};

}  // namespace filter
