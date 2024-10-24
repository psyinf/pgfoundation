#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace pg::foundation {

template <class T, typename... Arguments>
class GenericFactory
{
public:
    using CtorFunc = std::function<typename std::unique_ptr<typename std::remove_pointer_t<T>>(Arguments&&...)>;

    template <typename... FixedParameters>
    static auto proto(FixedParameters&&... fixed)
    {
        return [&fixed...](Arguments&&... args) {
            return std::make_unique<T>(std::forward<Arguments>(args)..., std::forward<FixedParameters>(fixed)...);
        };
    }

    static auto proto()
    {
        return [](Arguments&&... args) { return std::make_unique<T>(std::forward<Arguments>(args)...); };
    }

    GenericFactory() = default;
    ~GenericFactory() = default;

    /**
     * \brief register a prototype
     * \example:
     * GenericFactory<X> f;
     * f.registerPrototype("kitty", GenericFactory<X>::proto());
     */
    bool registerPrototype(std::string_view key, CtorFunc&& constructor_function)
    {
        auto ret = registeredConstructors.emplace(key, std::move(constructor_function));
        return ret.second;
    }

    /**
     * \brief	Gets module constructor by name
     * \param	type_name	Name of the type you want to build.
     * \return	The constructor of that type.
     */
    const CtorFunc& getPrototype(std::string_view type_name) const
    {
        return registeredConstructors.at(std::string(type_name));
    }

    /**
     * \brief Check if any prototype with a given name is registered
     * \param key
     */
    bool hasPrototypeName(std::string_view key) const { return registeredConstructors.contains(key); }

    /**
     * \brief construct an instance of the registered type
     * \param key
     */
    auto make(std::string_view key, Arguments&&... args) { return getPrototype(key)(std::forward<Arguments>(args)...); }

    /**
     * \brief Get all keys
     */
    auto getKeys() const { return std::views::keys(registeredConstructors); }

private:
    std::map<std::string, CtorFunc, std::less<>> registeredConstructors; ///< Mapping of key to constructors
};
} // namespace pg::foundation

/**
//example use
#include <iostream>

struct SomeParam{
    std::string name;
};

class RegisteredBaseType
{
public:
    RegisteredBaseType(const SomeParam& p1, int i)
    :p1(p1), i(i)
    {}

    ~RegisteredBaseType() = default;

    virtual void print()
    {
        std::cout <<"Base::print "  << p1.name << " " << i <<std::endl;
    }
 protected:
    SomeParam p1;
    int i;
};
class DerivedType : public RegisteredBaseType
{
    using RegisteredBaseType::RegisteredBaseType;
    void print() override
    {
       std::cout <<"Derived::print "  << p1.name << " "<< i << std::endl;
    }
};

using ExampleRegistry = GenericFactory<RegisteredBaseType, SomeParam&, int>;

int main(){
    SomeParam sp1{"sp1"};
    SomeParam sp2{"sp2"};
    ExampleRegistry reg;
    reg.registerPrototype("baseType", GenericFactory<RegisteredBaseType, SomeParam&, int >::proto());
    reg.registerPrototype("derivedType", GenericFactory<DerivedType, SomeParam&, int >::proto());

    auto b1 = reg.make("baseType", sp1, 7);
    auto b2 = reg.make("derivedType", sp2, 9);
    b1->print();
    b2->print();

 }*/