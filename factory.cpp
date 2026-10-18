#include "factory.h"

#include "errors.h"
#include "filters/filters.h"

#include <map>

namespace factory {
namespace {
using Check = void (*)(const std::vector<std::string_view>& params);
using Creator = std::unique_ptr<filter::Filter> (*)(const args::FilterSpec& filter_spec);
using Info = std::string_view (*)();

template <typename T>
std::unique_ptr<filter::Filter> Make(const args::FilterSpec& filter_spec) {
    return std::make_unique<T>(filter_spec.params);
}

const std::map<std::string_view, std::tuple<Check, Creator, Info>> CREATORS = {
    {"crop", {crop::Check, Make<crop::CropFilter>, crop::Info}},
    {"sharp", {sharp::Check, Make<sharp::SharpFilter>, sharp::Info}},
    {"edge", {edge::Check, Make<edge::EdgeFilter>, edge::Info}},
    {"gs", {gs::Check, Make<gs::GsFilter>, gs::Info}},
    {"neg", {neg::Check, Make<neg::NegFilter>, neg::Info}},
    {"blur", {blur::Check, Make<blur::BlurFilter>, blur::Info}},
    {"twirl", {twirl::Check, Make<twirl::TwirlFilter>, twirl::Info}},
};
}  // namespace

void CheckFilter(const args::FilterSpec& filter_spec) {
    const auto it = CREATORS.find(filter_spec.name);
    if (it == CREATORS.end()) {
        throw error::FilterError("Filter " + static_cast<std::string>(filter_spec.name) + " does not exist.");
    }
    get<0>(it->second)(filter_spec.params);
}

std::unique_ptr<filter::Filter> CreateFilter(const args::FilterSpec& filter_spec) {
    const auto it = CREATORS.find(filter_spec.name);
    if (it == CREATORS.end()) {
        throw error::FilterError("Filter " + static_cast<std::string>(filter_spec.name) + " does not exist.");
    }
    return get<1>(it->second)(filter_spec);
}

std::string_view FilterInfo(std::string_view name) {
    const auto it = CREATORS.find(name);
    if (it == CREATORS.end()) {
        throw error::Defect("Logic Error - FilterInfo " + static_cast<std::string>(name) + " does not exist.");
    }
    return get<2>(it->second)();
}

std::vector<std::string_view> KnownTypes() {
    std::vector<std::string_view> types;
    types.reserve(CREATORS.size());
    for (const auto& [type, creator] : CREATORS) {
        types.emplace_back(type);
    }
    return types;
}
}  // namespace factory
