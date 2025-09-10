#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "../include/FileSystemException.hpp"

namespace utility {

struct PathPrefix
{
    enum class StartType { INVALID, CURRENT, ROOT } type{StartType::INVALID};
    int ups{};             // number of "../"
    std::string rest;      // remaining path
};

[[nodiscard]] inline PathPrefix validatePath(const std::string& path)
{
    PathPrefix result;
    size_t pos{};

    if (path.empty()) throw InvalidPathException("Path cannot be empty");

    if (path[0] == '/') {
        result.type = PathPrefix::StartType::ROOT;
        pos = 1;
    }
    else if (path.compare(0, 2, "./") == 0 || path.compare(0, 3, "../") == 0 || path.compare(0, 1, ".") == 0) {
        result.type = PathPrefix::StartType::CURRENT;
        // pos will be handled in the loop below
    }
    else {
        result.type = PathPrefix::StartType::CURRENT; // plain relative path
    }

    while (pos < path.size()) {
        if (path.compare(pos, 2, "./") == 0) {
            pos += 2;
        }
        else if (path.compare(pos, 3, "../") == 0) {
            ++result.ups;
            pos += 3;
        }
        else if (path.compare(pos, 2, "..") == 0 && (pos + 2 == path.size() || path[pos + 2] == '/')) {
            ++result.ups;
            pos += 2;
            if (pos < path.size() && path[pos] == '/') ++pos;
        }
        else if (path.compare(pos, 1, ".") == 0 && (pos + 1 == path.size() || path[pos + 1] == '/')) {
            pos += 1;
            if (pos < path.size() && path[pos] == '/') ++pos;
        }
        else {
            break;
        }
    }

    if (result.type == PathPrefix::StartType::INVALID) {
        throw InvalidPathException(path);
    }

    if (pos < path.size()) {
        result.rest = path.substr(pos);
    }

    return result;
}

[[nodiscard]] inline std::vector<std::string> split(const std::string& path)
{
    std::stringstream ss{path};
    std::string s;
    std::vector<std::string> res;

    while (std::getline(ss, s, '/')) {
        if (!s.empty()) res.push_back(s);
    }

    return res;
}

struct KMPSolver
{
    [[nodiscard]] inline static bool solve(const std::string& text, const std::string& pattern)
    {
        std::size_t patternSize = pattern.size();
        std::vector<int> lps(patternSize);
        std::size_t i{1}, j{};
        
        while (i < patternSize) {
            if (pattern[i] == pattern[j]) {
                lps[i] = j + 1;
                ++i; ++j;
            }
            else if (j == 0) {
                ++i;
            }
            else {
                j = lps[j - 1];
            }
        }

        i = 0;
        j = 0;

        while (i < text.size()) {
            if (text[i] == pattern[j]) {
                ++i; ++j;
                if (j == patternSize) return true;
            }
            else if (j == 0) {
                ++i;
            }
            else {
                j = lps[j - 1];
            }
        }

        return false;
    }
};

} // namespace utility

// Optional test main
#ifdef TEST_PATHVALIDATOR
int main() {
    std::string paths[] = {"/../../.", ".././../../file.txt", "foo/bar"};
    for (auto& p : paths) {
        auto prefix = utility::validatePath(p);
        std::cout << p << " -> "
                  << (prefix.type == utility::PathPrefix::StartType::ROOT ? "ROOT" : "CURRENT")
                  << ", ups=" << prefix.ups
                  << ", rest='" << prefix.rest << "'\n";
    }
}
#endif
