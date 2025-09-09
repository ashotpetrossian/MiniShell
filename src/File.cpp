#include "../include/File.hpp"

void File::write(const std::string& message, bool append)
{
    if (!append) fileContent.clear();

    fileContent += message + "\n";
}