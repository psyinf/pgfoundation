#pragma once

namespace pg::foundation {
class ResourceLocatorBase
{
public:
    std::filesystem::path locate(this auto&& self, const std::string& uri) { return self.loc_impl(uri); }

    bool contains(this auto&& self, const std::string& uri) { return self.has_impl(uri); }
};

class IdentityResourceLocator : public ResourceLocatorBase
{
public:
    std::filesystem::path loc_impl(const std::string& uri) { return uri; }

    bool has_impl([[maybe_unused]] const std::string& uri) { return true; }
};

class MapppedResourceLocator : public ResourceLocatorBase
{
public:
    using UriMap = std::unordered_map<std::string, std::string>;

    MapppedResourceLocator(UriMap&& locations)
      : _locations(std::move(locations))
    {
    }

    void addLocation(this auto&& self, const std::string& uri, const std::string& path) { self.add_impl(uri, path); }

    bool hasLocation(this auto&& self, const std::string& uri) { return self.has_impl(uri); }

    std::string loc_impl(const std::string& uri) { return _locations.at(uri); }

    bool has_impl(const std::string& uri) { return _locations.contains(uri); }

private:
    UriMap _locations;
};

// resource locator that constructs a path from the URI
class PathResourceLocator : public ResourceLocatorBase
{
public:
    PathResourceLocator(const std::filesystem::path& basePath)
      : _basePath(basePath)
    {
    }

    std::filesystem::path loc_impl(const std::string& uri) { return _basePath / std::filesystem::path{uri}; }

private:
    std::filesystem::path _basePath;
};

} // namespace pg::foundation
