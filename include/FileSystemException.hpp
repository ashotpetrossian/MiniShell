#pragma once

#include <exception>
#include <string>

class FileSystemException : public std::exception
{
private:
    std::string message;
public:
    explicit FileSystemException(const std::string& msg) : message{msg} { }

    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class InvalidPathException : public FileSystemException
{
public:
    explicit InvalidPathException(const std::string& path)
        : FileSystemException("Invalid path: " + path) { }
};

class InvalidNameException : public FileSystemException
{
public:
    explicit InvalidNameException(const std::string& name)
        : FileSystemException("Invalid name: " + name) { }
};

class InvalidOptionException : public FileSystemException
{
public:
    explicit InvalidOptionException(const std::string& option)
        : FileSystemException("Invalid option: " + option) { }
};

class InvalidOperationException : public FileSystemException
{
public:
    explicit InvalidOperationException(const std::string& message)
        : FileSystemException("Invalid operation: " + message) { }
};

class DirectoryAlreadyExists : public FileSystemException
{
public:
    explicit DirectoryAlreadyExists(const std::string& dirName)
        : FileSystemException("Directory \'" + dirName + "\' already exists") { }
};

class DirectoryDoesNotExist : public FileSystemException
{
public:
    explicit DirectoryDoesNotExist(const std::string& dirName)
        : FileSystemException("Directory \'" + dirName + "\' does not exist") { }
};

class FileDoesNotExist : public FileSystemException
{
public:
    explicit FileDoesNotExist(const std::string& fileName)
        : FileSystemException("File \'" + fileName + "\' does not exist") { }
};

class FileAlreadyExists : public FileSystemException
{
public:
    explicit FileAlreadyExists(const std::string& fileName)
        : FileSystemException("File \'" + fileName + "\' already exists") { }
};

class DirectoryNotEmptyException : public FileSystemException
{
public:
    explicit DirectoryNotEmptyException(const std::string& dirName)
        : FileSystemException("Directory \'" + dirName + "\' is not empty") { }
};