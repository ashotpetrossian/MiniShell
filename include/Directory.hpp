#pragma once

#include "FileSystemNode.hpp"
#include "FileSystemException.hpp"
#include <unordered_map>
#include <algorithm>

/**
 * @brief Represents a directory in the file system.
 * 
 * A Directory can contain files and subdirectories. Provides operations
 * for creating, removing, and listing children.
 * 
 * Inherits from FileSystemNode and supports shared_from_this for parent-child
 * relationships.
 */
class Directory : public FileSystemNode, public std::enable_shared_from_this<Directory>
{
    friend class FileSystemManager;

public:
    /**
     * @brief Constructs a directory with the given name.
     * @param name Name of the directory.
     */
    explicit Directory(const std::string& name) : dirName{name} { }

    /**
     * @brief Gets the number of children in the directory.
     * @return Number of files and subdirectories.
     */
    virtual std::size_t getSize() const override;

    /**
     * @brief Returns the name of the directory.
     * @return Directory name as string.
     */
    virtual std::string getName() const override { return dirName; }

    /**
     * @brief Creates a new subdirectory.
     * @param name Name of the new directory.
     */
    void mkdir(const std::string& name);

    /**
     * @brief Removes an empty subdirectory.
     * @param name Name of the directory to remove.
     * @throws InvalidOperationException if the directory is not empty.
     */
    void rmEmptyDir(const std::string& name);

    /**
     * @brief Recursively removes a subdirectory and all its contents.
     * @param name Name of the directory to remove.
     */
    void rmEntireDir(const std::string& name);

    /**
     * @brief Removes a file from this directory.
     * @param name Name of the file to remove.
     * @throws FileDoesNotExists if the file does not exist.
     */
    void rmFile(const std::string& name);

    /**
     * @brief Creates a new file or updates the last modified time if it exists.
     * @param name Name of the file.
     */
    void createOrUpdateFile(const std::string& name);

    /**
     * @brief Lists the names of all children (files and directories).
     * @return Vector of child names.
     */
    std::vector<std::string> ls() const;

    /**
     * @brief Computes the full path from the root to this directory.
     * @return Absolute path as a string.
     */
    std::string getFullPath() const override;

    /**
     * @brief Checks if this node is a directory.
     * @return Always true for Directory.
     */
    virtual bool isDirectory() const override { return true; }

private:
    /**
     * @brief Adds a child node to this directory.
     * @param child Shared pointer to the child node.
     */
    void addChild(std::shared_ptr<FileSystemNode> child);

    /**
     * @brief Removes a child node from this directory.
     * @param name Name of the child to remove.
     */
    void removeChild(const std::string& name);

private:
    /// Map of child names to their corresponding nodes (files or directories).
    std::unordered_map<std::string, std::shared_ptr<FileSystemNode>> children;

    /// Name of this directory.
    std::string dirName;
};
