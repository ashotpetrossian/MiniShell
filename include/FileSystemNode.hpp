#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

class Directory;

/**
 * @brief Abstract base class for all file system nodes (files and directories).
 *
 * Provides a unified interface for operations common to both files and 
 * directories, such as retrieving the name, size, full path, and 
 * last-modified time.
 *
 * Design:
 * - Each node keeps a weak pointer to its parent directory to avoid 
 *   circular ownership.
 *
 * Inheritance:
 * - @see File for concrete file nodes.
 * - @see Directory for directory nodes.
 */
class FileSystemNode
{
protected:
    /// Weak pointer to parent directory (avoids cyclic references).
    std::weak_ptr<Directory> parent{};

public:
    /**
     * @brief Sets the parent directory of this node.
     * @param p Shared pointer to the parent directory.
     */
    void setParent(std::shared_ptr<Directory> p) { parent = p; }

    /**
     * @brief Gets the size of the node.
     * 
     * - For a file, returns its content size (in bytes).
     * - For a directory, returns the number of children.
     * 
     * @return The size of the node.
     */
    virtual std::size_t getSize() const = 0;

    /**
     * @brief Gets the name of the node.
     * @return Node name as a string.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Computes the full path of this node from the root.
     * @return The absolute path as a string.
     */
    virtual std::string getFullPath() const = 0;

    /**
     * @brief Checks if this node is a directory.
     * @return True if it is a directory, false if it is a file.
     */
    virtual bool isDirectory() const noexcept = 0;

    /**
     * @brief Virtual destructor for safe polymorphic deletion.
     */
    virtual ~FileSystemNode() = default; // implicitly noexcept
};
