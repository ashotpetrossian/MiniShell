#include "../include/Shell.hpp"
#include "FileSystemException.hpp"

void Shell::run()
{
    std::cout << "Shell run...\n";

    while (true) {
        std::cout << "[" << fsManager.getLastDirName() << "] $ ";
        std::string input;
        if (!std::getline(std::cin, input)) break;  // handle EOF (Ctrl+D)

        auto tokens = parser.parse(input);
        if (tokens.empty()) continue;

        try {
            auto command = parser.createCommand(tokens.front());
            if (command == nullptr) {
                std::cout << "Invalid Command\n";
                continue;
            }

            std::vector<std::string> args{tokens.begin() + 1, tokens.end()};
            if (!command->validate(args)) {
                std::cout << "Invalid arguments\n";
                continue;
            }

            command->execute(fsManager, args);
        }
        catch (const FileSystemException& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << "\n";
        }
    }
}