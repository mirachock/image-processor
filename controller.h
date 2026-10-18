#pragma once

#include "args.h"
#include "filter.h"
#include "image.h"

#include <memory>
#include <vector>

namespace controller {

class Controller {
public:
    explicit Controller(const std::vector<args::FilterSpec>& filter_specs);
    image::Image Apply(image::Image&& img) const;

private:
    std::vector<std::unique_ptr<filter::Filter>> chain_;
};

}  // namespace controller
