#pragma once
#include <any>
#include <sdlpp.hpp>
#include <string>
#include <unordered_map>

namespace pg::foundation {
using URI = std::string;

/**
 * A generic resource cache that allows to pass factory function to create/load the specified resource.
 * Resources are stored using std::any to allow for different types of resources to be stored in the same cache
 * The context is passed to the maker function to allow for context-specific resource loading. E.g. this could be the
 * base path for the resources
 */
class ResourceCache
{
public:
    ResourceCache() = default;

    bool has(const URI& uri) const { return _resources.contains(uri); }

    template <typename Resource>
    std::shared_ptr<Resource> get(const URI& uri)
    {
        return std::any_cast<std::shared_ptr<Resource>>(_resources.at(uri));
    }

    template <typename Resource>
    std::shared_ptr<Resource> retrieve(const URI& uri)
    {
        if (!has(uri)) { _resources[uri] = std::make_shared<Resource>(uri); }
        return std::any_cast<std::shared_ptr<Resource>>(_resources[uri]);
    }

    template <typename Resource, typename Maker, typename... Args>
    std::shared_ptr<Resource> retrieve(const URI& uri, Maker&& maker, Args... args)
    {
        if (!has(uri)) { _resources[uri] = std::make_shared<Resource>(std::move(maker(uri, args...))); }
        return std::any_cast<std::shared_ptr<Resource>>(_resources[uri]);
    }

    template <typename Resource, typename Maker>
    std::shared_ptr<Resource> retrieve(const URI& uri, Maker&& maker)
    {
        if (!has(uri)) { _resources[uri] = std::make_shared<Resource>(std::move(maker(uri))); }
        return std::any_cast<std::shared_ptr<Resource>>(_resources[uri]);
    }

private:
    std::unordered_map<URI, std::any> _resources;
};

/**
 * A typed resource cache that allows to pass factory function to create/load the specified resource.
 * It allows to store only a single type of resource in the cache
 */
template <typename Resource, typename Maker = std::function<Resource(const URI&)>>
class TypedResourceCache
{
    using URI = std::string;

public:
    TypedResourceCache(Maker&& maker)
      : _maker(maker)
    {
    }

    std::shared_ptr<Resource> load(const URI& uri)
    {
        if (!_resources.contains(uri))
        {
            // TODO: use std::filesystem
            _resources[uri] = std::make_shared<Resource>(std::move(_maker(uri)));
        }
        return _resources[uri];
    }

private:
    std::unordered_map<URI, std::shared_ptr<Resource>> _resources{};
    Maker                                              _maker;
};
} // namespace pg::foundation
