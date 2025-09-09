#pragma once

#include "FileSystemNode.hpp"

/**
 * @brief Represents a file in the file system.
 *
 * Stores the file name and its content. Provides operations to read, write, 
 * and get the size of the file. Inherits from FileSystemNode.
 */
class File : public FileSystemNode
{
public:
    /**
     * @brief Constructs a File with a given name and optional content.
     * @param name Name of the file.
     * @param content Initial content of the file (default empty).
     */
    File(const std::string& name, const std::string& content = "")
        : fileName{name}, fileContent{content} { }

    /**
     * @brief Gets the size of the file in bytes.
     * @return Size of the file content.
     */
    virtual std::size_t getSize() const override { return fileContent.size(); }

    /**
     * @brief Gets the file name.
     * @return Name of the file.
     */
    virtual std::string getName() const override { return fileName; }

    /**
     * @brief Gets the content of the file.
     * @return File content as a string.
     */
    std::string getContent() const { return fileContent; }

    /**
     * @brief Writes a message to the file.
     * @param message Message to write.
     * @param append If true, append to existing content; otherwise, overwrite.
     */
    void write(const std::string& message, bool append = false);

    /**
     * @brief Reads the file content.
     * @return File content as a string.
     */
    std::string read() const { return fileContent; }

    /**
     * @brief Returns the full path of the file.
     * @note Placeholder, actual path resolution handled by FileSystemManager.
     */
    virtual std::string getFullPath() const override { return ""; }

    /**
     * @brief Checks if this node is a directory.
     * @return Always false for files.
     */
    virtual bool isDirectory() const override { return false; }

private:
    std::string fileName;     ///< Name of the file
    std::string fileContent;  ///< Content of the file
};
