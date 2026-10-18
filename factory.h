#pragma once

#include "args.h"
#include "filter.h"

#include <memory>
#include <vector>

namespace factory {

void CheckFilter(const args::FilterSpec& filter_spec);

std::unique_ptr<filter::Filter> CreateFilter(const args::FilterSpec& filter_spec);

std::string_view FilterInfo(std::string_view name);

std::vector<std::string_view> KnownTypes();

}  // namespace factory
