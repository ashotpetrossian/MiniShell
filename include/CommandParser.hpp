#pragma once

#include "FileSystemManager.hpp"

#include <unordered_map>
#include <functional>
#include <sstream>
#include <fstream>

class Command;

/// @brief Parses and executes shell commands.
class CommandParser
{
public:
    /// @brief Constructs a CommandParser and registers all supported commands.
    CommandParser();

    /// @brief Parses the user input into a list of command and arguments.
    /// @param input The entire input string entered by the user.
    /// @return A vector of strings, where the first element is the command and the rest are arguments.
    std::vector<std::string> parse(const std::string& input);   

    /// @brief Creates a command object based on the command name.
    /// @param name The name of the command.
    /// @return A unique_ptr to the corresponding Command object, or nullptr if the command does not exist.
    std::unique_ptr<Command> createCommand(const std::string& name); 

private:
    /// @brief Stores command name to factory function mapping.
    std::unordered_map<std::string, std::function<std::unique_ptr<Command>()>> registry;
};

/// @brief Base class for all shell commands.
class Command
{
public:
    virtual ~Command() = default;

    /// @brief Checks if the provided arguments are valid for the command.
    /// @param args The list of arguments to validate.
    /// @return true if valid, false otherwise.
    virtual bool validate(const std::vector<std::string>& args) const noexcept = 0;

    /// @brief Executes the command using the provided FileSystemManager.
    /// @param fsManager The file system manager to operate on.
    /// @param args The arguments provided to the command.
    virtual void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) = 0;
};

/// @brief Prints the current working directory.
class PWDCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.empty(); }

    /// @brief Prints the current directory to stdout.
    void execute(FileSystemManager& fsManager, [[maybe_unused]] const std::vector<std::string>& args) override;
};

/// @brief Changes the current working directory.
class CDCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 1; }

    /// @brief Changes the current directory to the specified path.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Creates a new directory.
class MKDIRCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 1; }

    /// @brief Creates the specified directory.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Lists the contents of a directory.
class LSCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.empty() || args.size() == 1; }

    /// @brief Prints the contents of the specified directory (or current directory if none).
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Removes directories.
class RMDIRCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return !args.empty() && args.size() <= 2; }

    /// @brief Removes the specified directory, optionally recursively with -r.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Removes files.
class RMDCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 1; }

    /// @brief Deletes the specified file.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Creates files.
class TOUCHCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return !args.empty(); }

    /// @brief Creates the specified files.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Prints text or writes it to a file.
class ECHOCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return !args.empty(); }

    /// @brief Prints the text to stdout or redirects it to a file if > or >> is used.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Prints the contents of a file.
class CATCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 1; }

    /// @brief Prints the content of the specified file.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Copies files or directories.
class CPCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 2 || args.size() == 3; }

    /// @brief Copies a file or directory. Supports optional -r for recursive copy.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Moves or renames files or directories.
class MVCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 2 || args.size() == 3; }

    /// @brief Moves a file or directory. Supports optional -r for recursive move.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Searches for a pattern in files or directories.
class GREPCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 2 || args.size() == 3; }

    /// @brief Prints lines matching the pattern. Supports optional -r for recursive search.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};

/// @brief Converts a directory structure to JSON and writes it to a file.
class ToJsonCommand : public Command
{
public:
    bool validate(const std::vector<std::string>& args) const noexcept override { return args.size() == 3 && args[1] == ">"; }

    /// @brief Converts the specified directory to JSON and writes to the output file.
    void execute(FileSystemManager& fsManager, const std::vector<std::string>& args) override;
};
