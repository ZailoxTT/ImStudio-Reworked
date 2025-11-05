// utils/filesystem_utils.cpp
#include "filesystem_utils.h"
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>
    #pragma comment(lib, "shell32.lib")
#else
    #include <unistd.h>
    #include <pwd.h>
#endif

namespace ImStudio::FileSystem
{
  std::string GetProjectDirectory()
  {
    #ifdef _WIN32
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &path);
    if (SUCCEEDED(hr)) {
        std::wstring wpath(path);
        CoTaskMemFree(path);
        std::string result = std::string(wpath.begin(), wpath.end()) + "\\ImStudio-Reworked";
        return result;
      }
      // fallback
      //return "C:\\ImStudio-Reworked";
      #else
      const char* home = getenv("HOME");
      if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = (pw ? pw->pw_dir : nullptr);
      }
      return std::string(home ? home : "") + "/.ImStudio-Reworked";
      #endif
    }

    void EnsureProjectDirectory()
    {
        std::string dir = GetProjectDirectory();
        if (!fs::exists(dir)) {
            std::cout << "Creating directory: " << dir << std::endl;
            fs::create_directories(dir);
        }
    }

    std::string GetProjectFilePath(const std::string& filename)
    {
        EnsureProjectDirectory();
        std::string safe_name = filename;
        for (char& c : safe_name) {
            if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
                c = '_';
            }
        }
        if (safe_name.empty() || safe_name == ".") safe_name = "Untitled";
        if (fs::path(safe_name).extension() != ".json") {
            safe_name += ".json";
        }
        return GetProjectDirectory() + "/" + safe_name;
    }
}
