#include <pgf/caching/ResourceCache.hpp>
#include <catch2/catch_test_macros.hpp>

struct ResourceA
{
    std::string p;
};

struct ResourceB
{
    std::string p;
};

TEST_CASE("ResourceCache smoke tests", "[ResourceCache]")
{
    pg::foundation::ResourceCache cache;
    SECTION("Insert/Retrieve ResourceA")
    {
        auto resA = cache.retrieve<ResourceA>("a", [](const auto& p) { return ResourceA{p}; });
        REQUIRE(typeid(resA) == typeid(std::shared_ptr<ResourceA>));
        REQUIRE(resA);
        REQUIRE(resA.use_count() == 2);
        REQUIRE(resA->p == "a");
        auto resA2nd = cache.retrieve<ResourceA>("a", [](const auto& p) { return ResourceA{p}; });
        REQUIRE(resA2nd);
        REQUIRE(resA.use_count() == 3);
        REQUIRE(resA2nd.use_count() == 3);

        REQUIRE(resA2nd->p == "a");
        REQUIRE(resA.get() == resA2nd.get());
    }
    SECTION("Insert/Retrieve ResourceB")
    {
        auto resB = cache.retrieve<ResourceB>("bb", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB);
        REQUIRE(resB.use_count() == 2);
        REQUIRE(resB->p == "bb");
    }
}

TEST_CASE("ResourceCache multiple", "[ResourceCache]")
{
    pg::foundation::ResourceCache cache;
    SECTION("Insert/Retrieve A and B")
    {
        auto resB = cache.retrieve<ResourceB>("b", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB);
        REQUIRE(resB->p == "b");
        auto resB2 = cache.retrieve<ResourceB>("aa", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB2) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB2);
        REQUIRE(resB2->p == "aa");
    }
    SECTION("Re-register B with A's key")
    {
        auto resB = cache.retrieve<ResourceB>("b", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB);
        REQUIRE(resB->p == "b");
        REQUIRE_THROWS_AS(cache.retrieve<ResourceA>("b", [](const auto& p) { return ResourceA{p}; }),
                          std::bad_any_cast);
    }
}