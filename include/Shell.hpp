#pragma once

#include "FileSystemManager.hpp"
#include "CommandParser.hpp"
#include <csignal>

class Shell
{
private:
    FileSystemManager fsManager;
    CommandParser parser;

public:
    void run();
};