#pragma once

#include "argument.hpp"
#include "argument_register.hpp"

#include <functional>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stack>
#include <unordered_map>

namespace ws {

    class argument_parser
    {
    public:
        argument_parser(ws::argument_register const& argumentRegister):
            argumentRegister(argumentRegister)
        {}

        auto const& get_arguments() const noexcept { return this->argumentsMap; }
        auto const& get_argument(std::string_view argumentName, std::string_view argumentAlias = "") const
        {
            if (this->argumentsMap.contains(argumentName.data()))
                return this->argumentsMap.at(argumentName.data());
            else
                return this->argumentsMap.at(argumentAlias.data());
        }

        auto contains(std::string_view argumentName, std::string_view argumentAlias = "") const
        {
            return this->argumentsMap.contains(argumentName.data()) || this->argumentsMap.contains(argumentAlias.data());
        }

        void parse(auto const& arguments)
        {
            auto argumentsStack = this->tokenize(arguments);

            while (!argumentsStack.empty())
            {
                auto argumentToken = argumentsStack.top();
                argumentsStack.pop();

                if (argumentToken.has_depencies())
                {
                    this->check_if_required_argument_dependencies_are_present(argumentToken, argumentsStack);
                }

                auto const& name  = argumentToken.get_name();
                auto const& alias = argumentToken.get_alias();

                this->argumentsMap[!argumentToken.get_name().empty() ? name : alias] = argumentToken;
            }

            this->check_if_contains_required_arguments();
            this->check_if_contains_argument_with_missing_values();
        }

    private:
        std::stack<ws::argument> tokenize(auto const& arguments)
        {
            std::stack<ws::argument> argumentStack {};

            for (auto argument : arguments | std::views::transform([] (auto value) { return std::string_view(value); }))
            {
                auto argumentIterator = this->argumentRegister.find_registered_argument(argument);

                if (argument.starts_with("-"))
                {
                    if (argumentRegister.contains(argumentIterator))
                    {
                        argumentStack.push(*argumentIterator);
                        continue;
                    }

                    std::cerr << "IGNORING UNRECOGNIZED ARGUMENT: " << std::quoted(argument) << '\n';
                    continue;
                }

                if (argumentStack.empty())
                {
                    std::cerr << "IGNORING EXTRANEOUS ARGUMENT VALUE: " << std::quoted(argument) << '\n';
                    continue;
                }

                if (argumentStack.top().has_value() || argumentStack.top().get_kind() == ws::argument::kind::flag)
                {
                    std::cerr << "IGNORING EXTRANEOUS ARGUMENT VALUE: " << std::quoted(argument) << '\n';
                    continue;
                }

                switch (argumentStack.top().get_kind())
                {
                    case ws::argument::kind::boolean:
                    {
                        auto booleanValue = false;
                        std::stringstream(argument.data()) >> booleanValue;
                        argumentStack.top().set_value(booleanValue);
                        break;
                    }
                    
                    case ws::argument::kind::integer:
                    {
                        auto integerValue = 0;
                        std::stringstream(argument.data()) >> integerValue;
                        argumentStack.top().set_value(integerValue);
                        break;
                    }

                    case ws::argument::kind::decimal:
                    {
                        auto decimalValue = 0.0;
                        std::stringstream(argument.data()) >> decimalValue;
                        argumentStack.top().set_value(decimalValue);
                        break;
                    }

                    case ws::argument::kind::string:
                    {
                        argumentStack.top().set_value(argument.data());
                        break;
                    }

                    case ws::argument::kind::flag: break;
                }
            }

            return argumentStack;
        }

        void check_if_contains_required_arguments()
        {
            for (auto const& requiredArgument : this->argumentRegister.get_registered_arguments() | std::views::filter(&ws::argument::is_required))
            {
                if (!(this->argumentsMap.contains(requiredArgument.get_name()) || this->argumentsMap.contains(requiredArgument.get_alias())))
                {
                    std::cerr << "MISSING REQUIRED ARGUMENT " << std::quoted(requiredArgument.get_name()) << '\n';
                }
            }
        }

        void check_if_contains_argument_with_missing_values()
        {
            auto const arguments = this->argumentsMap | std::views::values;
            auto const argumentWihtMissingValue = std::ranges::find_if(arguments, std::not_fn(&ws::argument::has_value));

            if (argumentWihtMissingValue != arguments.end())
            {
                std::cerr << "MISSING VALUE FOR NON-FLAG ARGUMENT " << std::quoted((*argumentWihtMissingValue).get_name()) << '\n';
            }
        }

        void check_if_required_argument_dependencies_are_present(auto const& argumentToken, auto argumentsStack)
        {
            std::vector<std::string> arguments { argumentsStack.size() };

            while (!argumentsStack.empty())
            {
                auto argumentName  = argumentsStack.top().get_name();
                auto argumentAlias = argumentsStack.top().get_alias();
                arguments.push_back(argumentName.empty() ? argumentAlias : argumentName);
                argumentsStack.pop();
            }

            for (auto const& [dependencyName, dependencyAlias] : argumentToken.get_dependencies())
            {
                auto foundByName  = std::ranges::find(arguments, dependencyName) == arguments.end();
                auto foundByAlias = std::ranges::find(arguments, dependencyAlias) == arguments.end();

                if (!(foundByName || foundByAlias))
                {
                    std::stringstream stringStream {};
                    stringStream << "MISSING THE FOLLOWING ARGUMENT [ ";
                    stringStream << (foundByName ? dependencyName : dependencyAlias);
                    stringStream << " ] REQUIRED BY " << std::quoted(argumentToken.get_name());

                    std::cerr << stringStream.str() << '\n';
                }
            }
        }

        ws::argument_register const& argumentRegister;
        std::unordered_map<std::string, ws::argument> argumentsMap {};
    };

}
