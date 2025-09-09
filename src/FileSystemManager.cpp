#include "../include/FileSystemManager.hpp"
#include "../include/FileSystemException.hpp"
#include "../utility/Utils.hpp"
#include <algorithm>


FileSystemManager::FileSystemManager(): root{std::make_shared<Directory>("")}, cwd{root} {}

std::string FileSystemManager::pwd() const
{
    return cwd->getFullPath();
}

void FileSystemManager::cd(const std::string& path)
{
    cwd = navigateToDirectory(path, cwd);
}

std::vector<std::string> FileSystemManager::ls(const std::string& path) const
{
    auto node = cwd;
    if (!path.empty()) node = navigateToDirectory(path, node);
    return node->ls();
}

void FileSystemManager::mkdir(const std::string& name)
{
    cwd->mkdir(name);
}

void FileSystemManager::rmdir(const std::string& name, bool recursive)
{
    if (!recursive) cwd->rmEmptyDir(name);
    else cwd->rmEntireDir(name);
}

void FileSystemManager::rm(const std::string& name)
{
    cwd->rmFile(name);
}

void FileSystemManager::touch(const std::string& name)
{
    cwd->createOrUpdateFile(name);
}

void FileSystemManager::writeToFile(const std::string& fileName, const std::string& message, bool append)
{
    auto it = cwd->children.find(fileName);
    if (it == cwd->children.end()) {
        touch(fileName);
        it = cwd->children.find(fileName);
    }

    if (it->second->isDirectory()) {
        throw InvalidPathException(fileName + " is not a file");
    }

    auto fileNode = asNode<File>(it->second);
    fileNode->write(message, append);
}

std::string FileSystemManager::readFile(const std::string& fileName) const
{
    auto it = cwd->children.find(fileName);
    if (it == cwd->children.end()) {
        throw FileDoesNotExist(fileName);
    }

    if (it->second->isDirectory()) {
        throw InvalidPathException(fileName + " is not a file");
    }

    auto fileNode = asNode<File>(it->second);
    return fileNode->read();
}

void FileSystemManager::cp(const std::string& srcPath, const std::string& dstPath, bool recursive)
{
    auto [parentDir, fileName, srcNode] = resolveSource(srcPath, recursive);
    auto dstNode = resolveDestination(dstPath);

    if (!fileName.empty()) {
        auto fileNode = asNode<File>(parentDir->children[fileName]);
        auto newFile = std::make_shared<File>(fileNode->getName(), fileNode->getContent());
        dstNode->children[newFile->getName()] = newFile;
        newFile->setParent(dstNode);
    }
    else {
        validateCopyOrMove(srcNode, dstNode);
        copyDirectory(srcNode, dstNode);
    }
}

void FileSystemManager::copyDirectory(std::shared_ptr<Directory> srcNode, std::shared_ptr<Directory> dstNode)
{
    auto newDir = std::make_shared<Directory>(srcNode->getName());
    dstNode->children[newDir->getName()] = newDir;
    newDir->setParent(dstNode);

    for (auto& [name, node] : srcNode->children) {
        if (node->isDirectory()) {
            auto dirNode = asNode<Directory>(node);
            copyDirectory(dirNode, newDir);
        }
        else {
            auto fileNode = asNode<File>(node);
            auto newFile = std::make_shared<File>(fileNode->getName(), fileNode->getContent());
            newDir->children[newFile->getName()] = newFile;
            newFile->setParent(newDir);
        }
    }
}

void FileSystemManager::mv(const std::string& srcPath, const std::string& dstPath, bool recursive)
{
    auto [parentDir, fileName, srcNode] = resolveSource(srcPath, recursive);
    auto dstNode = resolveDestination(dstPath);

    if (!fileName.empty()) {
        auto fileNode = asNode<File>(parentDir->children[fileName]);
        parentDir->children.erase(fileName);
        dstNode->children[fileName] = fileNode;
        fileNode->setParent(dstNode);
    }
    else {
        validateCopyOrMove(srcNode, dstNode);

        auto srcParentNode = srcNode->parent.lock();
        srcParentNode->children.erase(srcNode->getName());
        dstNode->children[srcNode->getName()] = srcNode;
        srcNode->setParent(dstNode);
    }
}

auto FileSystemManager::resolveSource(const std::string& srcPath, bool recursive)
    -> std::tuple<std::shared_ptr<Directory>, std::string, std::shared_ptr<Directory>>
{
    auto srcNode = cwd;
    auto pathPrefix = utility::validatePath(srcPath);
    if (pathPrefix.type == utility::PathPrefix::StartType::ROOT) {
        srcNode = root;
    }
    else {
        int ups{pathPrefix.ups};
        while (srcNode != root && ups--) {
            srcNode = srcNode->parent.lock();
        }
    }

    std::vector<std::string> parts = utility::split(pathPrefix.rest);
    std::string fileName;
    for (std::size_t i{}; i < parts.size(); ++i) {
        if (parts[i].empty()) continue;

        auto it = srcNode->children.find(parts[i]);
        if (it == srcNode->children.end()) throw InvalidPathException(parts[i]);

        if (!it->second->isDirectory()) {
            if (i != parts.size() - 1) throw InvalidOperationException("File cannot contain a directory");
            fileName = parts[i];
            break;
        }

        srcNode = asNode<Directory>(it->second);
    }

    if (!fileName.empty() && recursive) throw InvalidOperationException("Cannot recursively copy/move a file");
    if (fileName.empty() && !recursive) throw InvalidOperationException("Cannot non-recursively copy/move a directory");

    if (!fileName.empty()) {
        return {srcNode, fileName, nullptr}; // parentDir, fileName, nullptr
    }
    else {
        return {nullptr, "", srcNode}; // nullptr, empty fileName, dirNode
    }
}

std::shared_ptr<Directory> FileSystemManager::resolveDestination(const std::string& dstPath)
{
    auto dstNode = cwd;
    auto pathPrefix = utility::validatePath(dstPath);
    if (pathPrefix.type == utility::PathPrefix::StartType::ROOT) {
        dstNode = root;
    }
    else {
        int ups{pathPrefix.ups};
        while (dstNode != root && ups--) {
            dstNode = dstNode->parent.lock();
        }
    }

    for (const auto& part : utility::split(pathPrefix.rest)) {
        if (part.empty()) continue;

        auto it = dstNode->children.find(part);
        if (it == dstNode->children.end()) throw InvalidPathException(part);
        if (!it->second->isDirectory()) throw InvalidPathException(part + " is not a directory");

        dstNode = asNode<Directory>(it->second);
    }

    return dstNode;
}

void FileSystemManager::validateCopyOrMove(const std::shared_ptr<Directory>& srcNode, const std::shared_ptr<Directory>& dstNode)
{
    // Prevents copying a directory into itself.
    if (srcNode == dstNode) throw InvalidOperationException("Cannot copy a directory into itself");
    
    // Protects your root directory from being copied — important for a mini FS.
    if (srcNode == root) throw InvalidOperationException("Cannot copy the root directory");
    
    // Prevents infinite recursion if /src → /src/subdir.
    auto current = dstNode;
    while (current) {
        if (current == srcNode) throw InvalidOperationException("Cannot copy a directory into its own subdirectory");
        current = current->parent.lock();
    }
    
    // Prevents overwriting or ambiguous copies.
    if (dstNode->children.contains(srcNode->getName())) {
        throw InvalidOperationException("Destination already contains a directory/file with the same name");
    }
}

std::shared_ptr<Directory> FileSystemManager::navigateToDirectory(const std::string& path, std::shared_ptr<Directory> startNode) const
{
    auto node = startNode;

    auto pathPrefix = utility::validatePath(path);
    if (pathPrefix.type == utility::PathPrefix::StartType::ROOT) {
        node = root;
    }
    else {
        int ups{pathPrefix.ups};
        while (node != root && ups--) {
            node = node->parent.lock();
        }
    }

    std::vector<std::string> actualPath = utility::split(pathPrefix.rest);
    for (const std::string& s : actualPath) {
        if (s.empty()) continue;

        auto it = node->children.find(s);
        if (it == node->children.end()) {
            throw DirectoryDoesNotExist(s);
        }
        
        if (!it->second->isDirectory()) {
            throw InvalidPathException(s + " is not a directory");
        }   

        auto nextDir = asNode<Directory>(node->children[s]);
        node = nextDir;
    }

    return node;
}

std::optional<std::vector<std::string>> FileSystemManager::grep(const std::string& path, const std::string& pattern, bool recursive) const
{
    auto dstNode = navigateToDirectory(path, cwd);
    std::vector<std::string> res;

    if (!recursive) {
        for (auto [name, child] : dstNode->children) {
            if (!child->isDirectory()) {
                auto fileNode = asNode<File>(child);
                bool found{utility::KMPSolver::solve(fileNode->getContent(), pattern)};
                if (found) res.push_back(name);
            }
        }
    }
    else {
        std::vector<std::string> resPath;
        dfsAndGrep(dstNode, pattern, resPath, res);
    }

    if (res.empty()) return std::nullopt;
    return res;
}

void FileSystemManager::dfsAndGrep(std::shared_ptr<Directory> node, const std::string& pattern, std::vector<std::string>& path, std::vector<std::string>& res) const
{
    path.push_back(node->getName());

    for (auto [name, child] : node->children) {
        if (!child->isDirectory()) {
            auto fileNode = asNode<File>(child);
            bool found{utility::KMPSolver::solve(fileNode->getContent(), pattern)};

            if (found) {
                std::string s;
                for (std::size_t i{}; i < path.size(); ++i) {
                    if (i > 0) s += "/";
                    s += path[i];
                }

                s += "/" + fileNode->getName();
                res.push_back(s);
            }
        }
        else {
            auto dirNode = asNode<Directory>(child);
            dfsAndGrep(dirNode, pattern, path, res);
        }
    }

    path.pop_back();
}

FileSystemManager::json FileSystemManager::convertToJson(const std::string& path) const
{
    auto node = navigateToDirectory(path, cwd);
    return directoryToJson(node);
}

FileSystemManager::json FileSystemManager::directoryToJson(std::shared_ptr<Directory> node) const
{
    
    json j;

    for (auto& [name, child] : node->children) {
        if (child->isDirectory()) {
            auto dirNode = asNode<Directory>(child);
            j[name] = directoryToJson(dirNode);  // recursive
        }
        else {
            auto fileNode = asNode<File>(child);
            j[name] = fileNode->getContent();    // file content
        }
    }

    return j;
}