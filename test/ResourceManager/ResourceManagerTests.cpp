#include <pgf/caching/ResourceManager.hpp>
#include <catch2/catch_test_macros.hpp>

struct ResourceA
{
    std::string p;
};

struct ResourceB
{
    std::string p;
};

namespace pg::foundation {

template <>
auto loadResource<ResourceA>(DataProvider& provider) -> ResourceA
{
    if (provider.getUri() == "RES_C") { throw std::runtime_error("No loader for RES_C"); }
    return ResourceA{provider.getUri()};
}

template <>
ResourceA loadResource(DataProvider& provider, int x)
{
    return ResourceA{provider.getUri() + std::to_string(x)};
}

} // namespace pg::foundation

static constexpr auto MappingProviderFactory = [](const std::string& uri) -> pg::foundation::DataProviderPtr {
    std::unordered_map<std::string, std::string> mapping{{"resA", "RES_A"}, {"resB", "RES_B"}, {"resC", "RES_C"}};
    return std::make_shared<pg::foundation::FileDataProvider>(mapping[uri]);
};

TEST_CASE("ResourceManager smoke tests", "[ResourceCache]")
{
    pg::foundation::ResourceManager resMan(MappingProviderFactory);

    SECTION("Insert/Retrieve ResourceA")
    {
        auto result = resMan.load<ResourceA>("resA");
        REQUIRE(typeid(result) == typeid(std::shared_ptr<ResourceA>));
        REQUIRE(result->p == "RES_A");
        REQUIRE(result.use_count() == 2);

        auto result2 = resMan.load<ResourceA>("resA");
        REQUIRE(result.get() == result2.get());
        REQUIRE(result.use_count() == 3); // 2 shared_ptrs and 1 in cache
    }

    SECTION("missing loader")
    {
        REQUIRE_THROWS_AS(resMan.load<ResourceA>("resC"), std::runtime_error);
    }

    SECTION("parameter pack")
    {
        auto result2 = resMan.load<ResourceA, int>("resA", 5);
        REQUIRE(result2->p == "RES_A5");
    }
}
