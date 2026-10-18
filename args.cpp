#include "args.h"

#include "errors.h"

namespace args {

Args ParseArgs(const std::vector<std::string_view>& params) {
    Args answer;
    if (params.size() < 2u) {
        throw error::ArgsError("Arguments are specified incorrectly.");
    }
    answer.input = params[0];
    answer.output = params[1];
    for (size_t i = 2; i < params.size(); ++i) {
        FilterSpec filter;
        if (params[i].size() <= 1 || !params[i].starts_with('-')) {
            throw error::ArgsError("Filter specified incorrectly (-<filter> <parameter1> <parameter2> ...)");
        }
        filter.name = params[i].substr(1);
        size_t j = i + 1;
        while (j < params.size() && !params[j].starts_with('-')) {
            filter.params.push_back(params[j]);
            ++j;
        }
        i = j - 1;
        answer.filters.push_back(filter);
    }
    return answer;
}

}  // namespace args
