#pragma once

#include <cassert>
#include <string>
#include <string>
#include <variant>
#include <vector>

namespace ws {

    template <class T>
    static constexpr auto argument_as(auto&& argument)
    {
        return std::get<T>(std::forward<decltype(argument)>(argument).get_value());
    }

    class argument
    {
    public:
        enum class level { optional, required };
        enum class kind  { flag, boolean, integer, decimal, string };
    
    public:
        argument() = default;

        argument(std::string_view argumentName, argument::level argumentLevel):
            name(argumentName), kind(argument::kind::flag), level(argumentLevel)
        {}

        argument(std::string_view argumentName, std::string_view argumentAlias, argument::level argumentLevel = argument::level::optional):
            name(argumentName), alias(argumentAlias), kind(argument::kind::flag), level(argumentLevel)
        {}

        argument(std::string_view argumentName, argument::kind argumentKind = argument::kind::flag, argument::level argumentLevel = argument::level::optional):
            name(argumentName), kind(argumentKind), level(argumentLevel)
        {}

        argument(std::string_view argumentName, std::string_view argumentAlias, argument::kind argumentKind, argument::level argumentLevel = argument::level::optional):
            name(argumentName), alias(argumentAlias), kind(argumentKind), level(argumentLevel)
        {}

        auto const& get_name() const noexcept { return this->name; }
        auto const& get_alias() const noexcept { return this->alias; }
        auto const& get_kind() const noexcept { return this->kind; }
        auto const& get_level() const noexcept { return this->level; }
        auto const& get_value() const noexcept { return this->value; }
        auto const& get_dependencies() const noexcept { return this->dependencies; }
        
        auto set_value(auto const& newValue) noexcept { this->value = newValue; }

        auto has_depencies() const noexcept { return this->dependencies.size(); }
        auto has_value() const noexcept
        {
            switch (this->kind)
            {
                case ws::argument::kind::boolean: return std::holds_alternative<bool>(this->value);
                case ws::argument::kind::integer: return std::holds_alternative<int>(this->value);
                case ws::argument::kind::decimal: return std::holds_alternative<double>(this->value);
                case ws::argument::kind::string: return std::holds_alternative<std::string>(this->value);
                case ws::argument::kind::flag: return true; 
            }

            assert(false && "UNREACHABLE");
        }

        auto is_required() const noexcept { return this->level == argument::level::required; }
        auto is_optional() const noexcept { return this->level == argument::level::optional; }

        auto& depends_on(auto const& argumentName, auto const& argumentAlias = "")
        {
            this->dependencies.emplace_back(argumentName, argumentAlias);
            return *this;
        }

    private:
        std::string name {};
        std::string alias {};
        argument::kind kind {};
        argument::level level {};

        std::variant<bool, int, double, std::string> value {};

        std::vector<std::pair<std::string, std::string>> dependencies {};
    };

}
