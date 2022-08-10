#pragma once
#include <string>
#include <vector>
#include <filesystem>

// 引用于colmap
// Join multiple paths into one path.
template <typename... T>
std::string joinPaths(T const&... paths);

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

template <typename... T>
std::string joinPaths(T const&... paths)
{
    std::filesystem::path result;
    int unpack[]{0, (result = result / std::filesystem::path(paths), 0)...};
    static_cast<void>(unpack);
    return result.string();
}