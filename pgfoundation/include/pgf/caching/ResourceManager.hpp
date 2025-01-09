#pragma once

#include <memory>
#include <string>
#include <functional>

#include <pgf/caching/ResourceCache.hpp>
#include <pgf/caching/DataProvider.hpp>

namespace pg::foundation {

template <typename T>
inline auto loadResource(DataProvider& provider) -> T
{
    static_assert(false, "No resource loader found for type T");
}

template <typename T, typename... Args>
inline auto loadResource(DataProvider& provider, Args... args) -> T
{
    static_assert(false, "No resource loader found for type T");
}

using DataProviderPtr = std::shared_ptr<DataProvider>;
using DataProviderFactory = std::function<pg::foundation::DataProviderPtr(const std::string&)>;

static constexpr auto DefaultDataProviderFactory = [](const std::string& uri) -> DataProviderPtr {
    return std::make_unique<FileDataProvider>(uri);
};

class ResourceManager
{
public:
    ResourceManager() = default;

    ResourceManager(pg::foundation::DataProviderFactory providerFactory = DefaultDataProviderFactory) noexcept
      : _providerFactory(providerFactory)
    {
    }

    template <class T>
    std::shared_ptr<T> load(const std::string& uri)
    {
        return _cache.retrieve<T>(uri, [uri, p = _providerFactory](const std::string& _) {
            return pg::foundation::loadResource<T>(*p(uri));
        });
    }

    template <class T, typename... Args>
    std::shared_ptr<T> load(const std::string& uri, Args&&... args)
    {
        return _cache.retrieve<T>(uri, [uri, p = _providerFactory, &args...]([[maybe_unused]] const std::string& _) {
            return pg::foundation::loadResource<T, Args...>(*p(uri), std::forward<Args>(args)...);
        });
    }

private:
    pg::foundation::DataProviderFactory _providerFactory;
    pg::foundation::ResourceCache       _cache;
};

class ResourceManagerMonostate
{
public:
    ResourceManager& get() { return instance; };

private:
    static inline ResourceManager instance{DefaultDataProviderFactory};
};
} // namespace pg::foundation
