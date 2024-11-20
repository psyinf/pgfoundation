#include <catch2/catch_test_macros.hpp>
#include <pgf/caching/GenericFactory.hpp>

class Simple
{
public:
    Simple(int i)
      : _i(i)
    {
    }

    int _i;
};

class SimpleTwo
{
public:
    SimpleTwo(int i, int j)
      : _i(i)
      , _j(j)
    {
    }

    int _i;
    int _j;
};

TEST_CASE("GenericFactory", "[SmokeTest]")
{
    // make a factory for the type Simple with an parameter to be supplied at construction
    auto factory = pg::foundation::GenericFactory<Simple, int>();
    // register the prototype with the key "simple"
    factory.registerPrototype("simple", pg::foundation::GenericFactory<Simple, int>::proto());
    // make an instance of Simple with the key "simple" and the parameter 42
    auto simple = factory.make("simple", 42);
    // check that the instance was created correctly#
    REQUIRE(simple.get() != nullptr);
    REQUIRE(simple->_i == 42);
}

TEST_CASE("GenericFactory", "[Simple Fixed Parameter]")
{
    // make a factory for the type Simple with an parameter to be supplied at construction
    auto factory = pg::foundation::GenericFactory<Simple>();
    // register the prototype with the key "simple"
    factory.registerPrototype("simple", pg::foundation::GenericFactory<Simple>::proto(42));
    // make an instance of Simple with the key "simple" and the parameter 42
    auto simple = factory.make("simple");
    // check that the instance was created correctly#
    REQUIRE(simple.get() != nullptr);
    REQUIRE(simple->_i == 42);
}

TEST_CASE("GenericFactory", "[Multiple  Parameter]")
{
    // make a factory for the type Simple with an parameter to be supplied at construction
    auto factory = pg::foundation::GenericFactory<SimpleTwo, int, int&>();
    // register the prototype with the key "simple"
    factory.registerPrototype("simple", pg::foundation::GenericFactory<SimpleTwo, int, int&>::proto());
    // make an instance of Simple with the key "simple" and the parameter 42, also a reference to 43
    int  j = 43;
    auto simple = factory.make("simple", 42, j);
    // check that the instance was created correctly#
    REQUIRE(simple.get() != nullptr);
    REQUIRE(simple->_i == 42);
    REQUIRE(simple->_j == 43);
}

TEST_CASE("GenericFactory", "[Multiple  Mixed Parameter]")
{
    // make a factory for the type Simple with an parameter to be supplied at construction
    auto factory = pg::foundation::GenericFactory<SimpleTwo, int>();
    // register the prototype with the key "simple"
    int j = 43;
    factory.registerPrototype("simple", pg::foundation::GenericFactory<SimpleTwo, int>::proto(j));
    // make an instance of Simple with the key "simple" and the parameter 42, also a reference to 43

    auto simple = factory.make("simple", 42);
    // check that the instance was created correctly#
    REQUIRE(simple.get() != nullptr);
    REQUIRE(simple->_i == 42);
    REQUIRE(simple->_j == 43);
}
