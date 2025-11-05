// utils/filesystem_utils.h
#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace ImStudio::FileSystem
{
    std::string GetProjectDirectory();

    void EnsureProjectDirectory();

    std::string GetProjectFilePath(const std::string& filename);
}
