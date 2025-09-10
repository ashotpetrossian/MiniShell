#include "../include/Directory.hpp"
#include "../include/File.hpp"

std::size_t Directory::getSize() const
{
    std::size_t res{};
    for (const auto& [_, node] : children) {
        ++res;
        if (node->isDirectory()) res += node->getSize();
    }

    return res;
}

void Directory::mkdir(const std::string& name)
{
    if (name.empty() || name.starts_with(".") || name.find('/') != std::string::npos) {
        throw InvalidNameException(name);
    }

    if (children.contains(name)) {
        throw DirectoryAlreadyExists(name);
    }

    auto newDirectory = std::make_shared<Directory>(name);
    newDirectory->setParent(shared_from_this());
    children[name] = newDirectory;
}

void Directory::rmEmptyDir(const std::string& name)
{
    auto it = children.find(name);
    if (it == children.end()) {
        throw DirectoryDoesNotExist(name);
    }

    auto node = it->second;
    if (!node->isDirectory()) {
        throw InvalidOperationException("Target is not a directory: " + name);
    }

    if (node->getSize() > 0) {
        throw DirectoryNotEmptyException(name);
    }

    removeChild(name);
}

void Directory::rmEntireDir(const std::string& name)
{
    auto it = children.find(name);
    if (it == children.end()) {
        throw DirectoryDoesNotExist(name);
    }

    auto node = it->second;
    if (!node->isDirectory()) {
        throw InvalidOperationException("Target is not a directory: " + name);
    }
    
    removeChild(name);
}

void Directory::rmFile(const std::string& name)
{
    auto it = children.find(name);
    if (it == children.end()) {
        throw FileDoesNotExist(name);
    }

    auto node = it->second;
    if (node->isDirectory()) {
        throw InvalidOperationException("Target is not a file: " + name);
    }

    removeChild(name);
}

void Directory::removeChild(const std::string& name) noexcept
{
    children.erase(name);
}

void Directory::createOrUpdateFile(const std::string& name)
{
    auto it = children.find(name);
    if (it != children.end()) {
        if (it->second->isDirectory()) throw InvalidOperationException("Directory with name: " + name + " already exists");
        return;
    }

    auto newFile = std::make_shared<File>(name);
    newFile->setParent(shared_from_this());
    children[name] = newFile;
}


void Directory::addChild(std::shared_ptr<FileSystemNode> child)
{
    const std::string& childName{child->getName()};
    if (children.contains(childName)) {
        throw InvalidOperationException("Child already exists: " + childName);
        return;
    }

    child->setParent(shared_from_this());
    children[childName] = child;
}

std::string Directory::getFullPath() const
{
    auto node = shared_from_this();
    std::vector<std::string> segments;

    while (node != nullptr) {
        segments.push_back(node->getName());
        node = node->parent.lock();
    }

    std::string res{"/"};
    for (auto it{segments.rbegin()}; it != segments.rend(); ++it) {
        if (!it->empty()) res += *it + "/";
    }

    if (res.size() > 1) res.pop_back();

    return res;
}

std::vector<std::string> Directory::ls() const
{
    std::vector<std::string> res;
    for (const auto& [childName, _] : children) {
        res.push_back(childName);
    }

    return res;
}