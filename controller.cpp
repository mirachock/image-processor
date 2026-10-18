#include "controller.h"

#include "factory.h"

namespace controller {

Controller::Controller(const std::vector<args::FilterSpec>& filter_specs) {
    for (const args::FilterSpec& i : filter_specs) {
        factory::CheckFilter(i);
    }
    chain_.reserve(filter_specs.size());
    for (const args::FilterSpec& i : filter_specs) {
        chain_.emplace_back(factory::CreateFilter(i));
    }
}

image::Image Controller::Apply(image::Image&& image) const {
    for (const std::unique_ptr<filter::Filter>& filter : chain_) {
        image = filter->Apply(std::move(image));
    }
    return image;
}

}  // namespace controller
