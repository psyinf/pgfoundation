#pragma once
#include <string>
#include <hash>
namespace pg::foundation {
struct URI
{
    URI(const std::string& uri_str) { uri = uri_str; }

    URI(std::string_view uri_str) { uri = uri_str; }

    URI(const char* uri_str) { uri = {uri_str}; }

    operator std::string() const { return uri; };

    bool operator==(const URI& other) const { return uri == other.uri && type == other.type; }

    std::string uri;
    std::string type;
};
} // namespace pg::foundation

namespace std {
template <>
struct hash<pg::foundation::URI>
{
    size_t operator()(const pg::foundation::URI& u) const
    {
        // Compute individual hash values for two data members and combine them using XOR and bit shifting
        return std::hash<std::string>()(u.type + u.uri);
    }
};
} // namespace std
