#pragma once

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

/**
 * Small helper facilities to transform YAML to JSON
 * IDEA: see https://github.com/mircodezorzi/tojson/blob/master/tojson.hpp
 **/
namespace pg::foundation {
namespace internal {

inline nlohmann::json parse_scalar(const YAML::Node& node)
{
    if (int i; YAML::convert<int>::decode(node, i)) { return i; }
    if (double d; YAML::convert<double>::decode(node, d)) { return d; }
    if (bool b; YAML::convert<bool>::decode(node, b)) { return b; };
    if (std::string s; YAML::convert<std::string>::decode(node, s)) { return s; };

    return nullptr;
}
} // namespace internal

inline nlohmann::json yaml2json(const YAML::Node& root)
{
    nlohmann::json result_node{};

    switch (root.Type())
    {
    case YAML::NodeType::Null:
        break;
    case YAML::NodeType::Scalar:
        return internal::parse_scalar(root);
    case YAML::NodeType::Sequence:
        for (auto&& node : root)
        {
            result_node.emplace_back(yaml2json(node));
        }
        break;
    case YAML::NodeType::Map:
        for (auto&& it : root)
        {
            result_node[it.first.as<std::string>()] = yaml2json(it.second);
        }
        break;
    default:
        throw std::invalid_argument(std::format("Unkown yaml node type {0} cannot be transformed to JSON",
                                                root.Tag() /* magic_enum::enum_name(root.Type()*/));
        break;
    }
    return result_node;
}
} // namespace pg::foundation