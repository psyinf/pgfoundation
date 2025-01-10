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
using DataProviderFactory = std::function<pg::foundation::DataProviderPtr(const URI&)>;

static constexpr auto DefaultDataProviderFactory = [](const URI& uri) -> DataProviderPtr {
    return std::make_unique<FileDataProvider>(uri, true);
};

class ResourceManager
{
public:
    ResourceManager(pg::foundation::DataProviderFactory providerFactory = DefaultDataProviderFactory) noexcept
      : _providerFactory(providerFactory)
    {
    }

    template <class T>
    std::shared_ptr<T> load(const URI& uri)
    {
        return _cache.retrieve<T>(uri, [uri, p = _providerFactory](const std::string& _) {
            return pg::foundation::loadResource<T>(*p(uri));
        });
    }

    template <class T, typename... Args>
    std::shared_ptr<T> load(const URI& uri, Args&&... args)
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
