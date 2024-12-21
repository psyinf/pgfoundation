#include <ranges>
#include <regex>
#include <string>
#include <pgf/strings/StringTools.hpp>

static std::string wildcardToRegex(const std::string& s)
{
    std::string res = std::regex_replace(s, std::regex("\\."), "\\.");
    res = std::regex_replace(res, std::regex("\\?"), ".");

    return std::regex_replace(res, std::regex("\\*"), ".*");
}

bool pg::foundation::strings::matches(const std::string& s, const std::string& pattern)
{
    std::regex pattern_regex(pattern);
    return std::regex_search(std::string(s), pattern_regex);
}

bool pg::foundation::strings::matchesWildCard(const std::string& s,
                                              const std::string& wildcardPattern,
                                              bool               caseSensitive /*  =true*/)
{
    return std::regex_match(s, std::regex(wildcardToRegex(wildcardPattern)));
}

std::vector<std::string_view> pg::foundation::strings::tokenize(std::string_view       str,
                                                                const std::string_view delims /*= " "*/)
{
    std::vector<std::string_view> output;
    size_t                        first{0};

    while (first < str.size())
    {
        const auto second = str.find_first_of(delims, first);

        if (first != second) output.emplace_back(str.substr(first, second - first));

        if (second == std::string_view::npos) break;

        first = second + 1;
    }

    return output;
}

std::string pg::foundation::strings::toLower(std::string_view str)
{
    // return std::ranges::transform_view(str, [](unsigned char c) { return std::tolower(c); });
    std::string result;
    result.reserve(str.size());

    std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    }); // OK
    return result;
}
