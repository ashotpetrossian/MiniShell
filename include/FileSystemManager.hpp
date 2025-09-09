#pragma once

#include "FileSystemNode.hpp"
#include "Directory.hpp"
#include "File.hpp"
#include "json.hpp"

#include <optional>

/**
 * @brief Manages a virtual file system with directories and files.
 */
class FileSystemManager {
public:
    using json = nlohmann::json;

private:
    std::shared_ptr<Directory> root;  /**< Root directory of the file system */
    std::shared_ptr<Directory> cwd;   /**< Current working directory */

private:
    /**
     * @brief Copies the contents of one directory into another.
     * @param src Source directory to copy from.
     * @param dst Destination directory to copy into.
     */
    void copyDirectory(std::shared_ptr<Directory> src, std::shared_ptr<Directory> dst);

    /**
     * @brief Resolves the source path for copy or move operations.
     * @param srcPath Path to the source.
     * @param recursive Whether the operation is recursive.
     * @return Tuple containing source directory, name of the node, and parent directory.
     */
    auto resolveSource(const std::string& srcPath, bool recursive)
        -> std::tuple<std::shared_ptr<Directory>, std::string, std::shared_ptr<Directory>>;

    /**
     * @brief Resolves the destination directory for copy or move operations.
     * @param dstPath Path to the destination.
     * @return Pointer to the destination directory.
     */
    std::shared_ptr<Directory> resolveDestination(const std::string& dstPath);

    /**
     * @brief Validates that a copy or move operation can be performed.
     * @param srcNode Source node to copy/move.
     * @param dstNode Destination node.
     */
    void validateCopyOrMove(const std::shared_ptr<Directory>& srcNode, const std::shared_ptr<Directory>& dstNode);

    /**
     * @brief Navigates to a directory starting from a given node.
     * @param path Path to navigate to.
     * @param startNode Directory to start navigation from.
     * @return Pointer to the resolved directory.
     */
    std::shared_ptr<Directory> navigateToDirectory(const std::string& path, std::shared_ptr<Directory> startNode) const;

    /**
     * @brief Recursively searches for a pattern in files/directories.
     * @param node Current directory node.
     * @param pattern Pattern to search for.
     * @param path Current path vector.
     * @param res Vector to store matching paths.
     */
    void dfsAndGrep(std::shared_ptr<Directory> node, const std::string& pattern, std::vector<std::string>& path, std::vector<std::string>& res) const;

    /**
     * @brief Safely casts a FileSystemNode to the specified derived type.
     *
     * This function wraps std::dynamic_pointer_cast with an additional runtime check.
     * If the cast fails, it throws a std::runtime_error with a descriptive message,
     * ensuring that invalid type assumptions are caught early.
     *
     * @tparam T The target type to cast to (e.g., File, Directory).
     * @param node The FileSystemNode pointer to cast.
     * @return std::shared_ptr<T> A shared pointer to the node cast to type T.
     *
     * @throws std::runtime_error If the node cannot be cast to the requested type T.
     *
     * @note Use this helper to reduce repetitive dynamic_pointer_cast + null-check code.
     */
    template <typename T>
    std::shared_ptr<T> asNode(const std::shared_ptr<FileSystemNode>& node) const
    {
        auto ptr = std::dynamic_pointer_cast<T>(node);
        if (!ptr) throw std::runtime_error("Expected node of type: " + std::string(typeid(T).name()));
        return ptr;
    }
public:
    /**
     * @brief Constructs the FileSystemManager and initializes the root and cwd.
     */
    FileSystemManager();

    // Navigation

    /**
     * @brief Returns the current working directory as a string.
     * @return Full path of cwd.
     */
    std::string pwd() const;

    /**
     * @brief Changes the current working directory.
     * @param path Path to change to.
     */
    void cd(const std::string& path);

    /**
     * @brief Lists the contents of the specified directory.
     * @param path Path of the directory to list.
     * @return Vector of names of files and directories.
     */
    std::vector<std::string> ls(const std::string& path) const;

    // File/Directory operations

    /**
     * @brief Creates a new directory in the current directory.
     * @param name Name of the directory.
     */
    void mkdir(const std::string& name);

    /**
     * @brief Removes a directory, optionally recursively.
     * @param name Name of the directory.
     * @param option Optional flag, e.g., "-r".
     */
    void rmdir(const std::string& name, bool recursive = false);

    /**
     * @brief Removes a file.
     * @param name Name of the file.
     */
    void rm(const std::string& name);

    /**
     * @brief Creates a new file in the current directory.
     * @param name Name of the file.
     */
    void touch(const std::string& name);

    /**
     * @brief Writes text to a file.
     * @param fileName File to write to.
     * @param message Text to write.
     * @param append If true, appends to the file; otherwise overwrites.
     */
    void writeToFile(const std::string& fileName, const std::string& message, bool append = false);

    /**
     * @brief Reads the content of a file.
     * @param fileName File to read.
     * @return File contents as a string.
     */
    std::string readFile(const std::string& fileName) const;

    /**
     * @brief Searches for a pattern in files/directories.
     * @param path Path to search in.
     * @param pattern Pattern to search for.
     * @param recursive Whether to search recursively.
     * @return Optional vector of matching paths. Empty if no matches.
     */
    std::optional<std::vector<std::string>> grep(const std::string& path, const std::string& pattern, bool recursive = false) const;

    // Copy/Move

    /**
     * @brief Copies a file or directory.
     * @param src Source path.
     * @param dst Destination path.
     * @param recursive Whether to copy recursively.
     */
    void cp(const std::string& src, const std::string& dst, bool recursive = false);

    /**
     * @brief Moves a file or directory.
     * @param src Source path.
     * @param dst Destination path.
     * @param recursive Whether to move recursively.
     */
    void mv(const std::string& src, const std::string& dst, bool recursive = false);

    /**
     * @brief Converts a directory or file structure to JSON.
     * @param path Path to the directory.
     * @return JSON representation of the directory.
     */
    json convertToJson(const std::string& path) const;

    /**
     * @brief Converts a Directory object to JSON.
     * @param node Directory to convert.
     * @return JSON object.
     */
    json directoryToJson(std::shared_ptr<Directory> node) const;

    // Specific

    /**
     * @brief Returns the name of the current directory.
     * @return Name of cwd.
     */
    std::string getLastDirName() const { return cwd->getName(); }
};
