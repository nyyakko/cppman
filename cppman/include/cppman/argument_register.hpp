#pragma once

#include "argument.hpp"

#include <vector>
#include <algorithm>
#include <ranges>

namespace ws {

    class argument_register
    {
    public:
        auto const& get_registered_arguments() const { return this->arguments; }

        auto find_registered_argument_by_name(std::string_view argumentName) const
        {
            return std::ranges::find(this->arguments, argumentName, &ws::argument::get_name);
        }

        auto find_registered_argument_by_alias(std::string_view argumentAlias) const
        {
            return std::ranges::find(this->arguments, argumentAlias, &ws::argument::get_alias);
        }

        auto find_registered_argument(std::string_view argumentNameorAlias) const
        {
            auto foundByName  = this->find_registered_argument_by_name(argumentNameorAlias);
            auto foundByAlias = this->find_registered_argument_by_alias(argumentNameorAlias);

            return (foundByName != this->arguments.end()) ? foundByName : 
                        (foundByAlias != this->arguments.end()) ? foundByAlias : this->arguments.end();
        }

        auto contains(std::string_view argumentNameorAlias) const
        {
            return find_registered_argument(argumentNameorAlias) != this->arguments.end();
        }

        auto contains(auto const& argumentIterator) const
        {
            return argumentIterator != std::ranges::end(this->arguments);
        }

        auto& register_argument(auto&& ... parameters)
        {
            this->arguments.emplace_back(parameters...);
            return this->arguments.back();
        }

    private:
        std::vector<ws::argument> arguments {};
    };

}
