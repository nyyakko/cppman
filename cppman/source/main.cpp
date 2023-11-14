#include "cppman/argument.hpp"
#include "cppman/argument_parser.hpp"
#include "cppman/argument_register.hpp"

#include <print>
#include <ranges>
#include <span>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std::literals;

std::string setup_project_language(ws::argument_parser const& argumentParser)
{
    std::unordered_map<std::string_view, std::string_view> availableLanguages
    {
        { "c++"sv, "CXX"sv },
        { "c"sv,   "C"sv   }
    };

    std::string language {};

    if (argumentParser.contains("--language", "-L"))
    {
        auto selectedLanguage = ws::argument_as<std::string>(argumentParser.get_argument("--language", "-L"));

        if (!availableLanguages.contains(std::ranges::to<std::string>(selectedLanguage | std::views::transform(::tolower))))
        {
            std::println("The language \"{}\" isn't available.", selectedLanguage);
            std::exit(EXIT_FAILURE);
        }

        selectedLanguage = availableLanguages.at(selectedLanguage);
    }
    else
    {
        return "CXX";
    }

    return language;
}

std::string setup_project_language_standard(ws::argument_parser const& argumentParser, auto const& projectLanguage)
{
    std::unordered_map<std::string_view, std::vector<std::string_view>> availableStandards
    {
        { "CXX"sv, { "23"sv, "20"sv, "17"sv, "14"sv, "11"sv } },
        { "C"sv,   { "23"sv, "17"sv, "11"sv, "99"sv, } }
    };

    std::string standard {};

    if (argumentParser.contains("--standard", "-S"))
    {
        auto selectedStandard = ws::argument_as<std::string>(argumentParser.get_argument("--standard", "-S"));

        if (!std::ranges::contains(availableStandards.at(selectedStandard), selectedStandard))
        {
            std::println("The standard \"{}\" isn't for the language \"{}\".", selectedStandard, projectLanguage);
            std::exit(EXIT_FAILURE);
        }

        standard = selectedStandard;
    }
    else
    {
        return availableStandards.at(projectLanguage).front().data();
    }

    return standard;
}

std::string setup_project_template(ws::argument_parser const& argumentParser, auto const& projectLanguage)
{
    std::string projectTemplate {};

    if (argumentParser.contains("--template", "-T"))
    {
        projectTemplate = "templates\\"s + projectLanguage + "\\"s + ws::argument_as<std::string>(argumentParser.get_argument("--template", "-T"));
    }
    else
    {
        projectTemplate = "templates\\"s + projectLanguage + "\\console"s;
    }

    if (!std::filesystem::exists(projectTemplate))
    {
        std::println("The project template \"{}\" doesn't exist.", projectTemplate);
        std::exit(EXIT_FAILURE);
    }

    return projectTemplate;
}

void project_copy_files_recursively(std::filesystem::path from, std::filesystem::path to)
{
    static const auto projectName = to.filename().string();

    for (auto const& entry : std::filesystem::directory_iterator(from))
    {
        if (!std::filesystem::is_directory(entry))
        {
            std::filesystem::copy(entry, to);
            continue;
        }

        auto directoryName = entry.path().filename().string();

        if (directoryName == "!PROJECT!")
        {
            directoryName = projectName;
        }

        auto workingDirectory = std::filesystem::path(to).append(directoryName);
        std::filesystem::create_directory(workingDirectory);
        project_copy_files_recursively(entry, workingDirectory);
    }
}

void project_replace_wildcards_recursively(std::filesystem::path projectPath, std::unordered_map<std::string_view, std::string_view> const& wildcards)
{
    for (auto const& entry : std::filesystem::directory_iterator(projectPath))
    {
        if (std::filesystem::is_directory(entry))
        {
            project_replace_wildcards_recursively(entry, wildcards);
            continue;
        }

        std::stringstream fileContentStream {};
        fileContentStream << std::fstream(entry.path()).rdbuf();

        auto content = fileContentStream.str();

        for (auto wildcard : wildcards | std::views::keys)
        {
            while (auto wildcardPosition = content.find(wildcard))
            {
                if (wildcardPosition == std::string::npos)
                {
                    break;
                }
                
                auto first = std::next(content.begin(), static_cast<int>(wildcardPosition));
                auto last  = std::next(first, static_cast<int>(wildcard.size()));

                content.replace(first, last, wildcards.at(wildcard));
            }
        }

        std::fstream fileStream { entry.path(), std::ios::in | std::ios::out | std::ios::trunc };

        fileStream << content;
    }
}

void create_project_from_template(std::filesystem::path projectTemplate, std::string_view projectName, std::string_view projectLanguage, std::string_view projectStandard)
{
    if (!std::filesystem::exists(projectName))
    {
        std::filesystem::create_directory(projectName);
    }

    std::unordered_map<std::string_view, std::string_view> wildcards
    {
        { "!PROJECT!", projectName },
        { "!LANGUAGE!", projectLanguage },
        { "!STANDARD!", projectStandard },
    };

    project_copy_files_recursively(projectTemplate, projectName);
    project_replace_wildcards_recursively(projectName, wildcards);

    std::println("Project {} created successfully!", projectName);
}

int main(int argumentCount, char const** argumentValues)
{
    std::span<const char*> arguments(argumentValues, static_cast<std::size_t>(argumentCount));

    ws::argument_register argumentRegister {};

    argumentRegister.register_argument("--help", "-H");
    argumentRegister.register_argument("--name", "-N",     ws::argument::kind::string, ws::argument::level::required);
    argumentRegister.register_argument("--template", "-T", ws::argument::kind::string);
    argumentRegister.register_argument("--language", "-L", ws::argument::kind::string);
    argumentRegister.register_argument("--standard", "-S", ws::argument::kind::string);

    ws::argument_parser argumentParser { argumentRegister };
    argumentParser.parse(arguments | std::views::drop(1));

    if (argumentParser.contains("--help", "-H") || !argumentParser.contains("--name", "-N"))
    {
        std::println("Usage:\n");
        std::println("  {} --name <name> --template <template> --language <language> --standard <standard>\n", arguments.front());
        std::println("    Parameters:\n");
        std::println("      --name[-N] required, the project's name");
        std::println("      --template[-T] optional, the template used for creating the project. defaults to console.");
        std::println("      --language[-L] optional, the language used in the project. defaults to c++.");
        std::println("      --standard[-S] optional, the standard used in the project. defaults to latest.\n");

        return EXIT_FAILURE;
    }

    auto projectName     = ws::argument_as<std::string>(argumentParser.get_argument("--name", "-N"));
    auto projectLanguage = setup_project_language(argumentParser);
    auto projectStandard = setup_project_language_standard(argumentParser, projectLanguage);
    auto projectTemplate = setup_project_template(argumentParser, projectLanguage);

    std::println("Project name....: {}", projectName);
    std::println("Project language: {} ({})", projectLanguage, projectStandard);
    std::println("Project template: {}\n", projectTemplate);

    create_project_from_template(projectTemplate, projectName, projectLanguage, projectStandard);
}

