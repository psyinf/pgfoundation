#pragma once

#include <filesystem>
#include <vector>

namespace pg::foundation {

static bool isLeafDirectory(const std::filesystem::path& dir)
{
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.is_directory()) { return false; }
    }
    return true;
}

static std::vector<std::filesystem::path> getLeafSubDirectories(const std::filesystem::path& path)
{
    std::vector<std::filesystem::path> subDirectories;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory() && entry.isLeafDirectory()) { subDirectories.push_back(entry.path()); }
    }
    return subDirectories;
}

} // namespace pg::foundation