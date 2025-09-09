#include "../include/CommandParser.hpp"

std::vector<std::string> CommandParser::parse(const std::string& input)
{
    std::stringstream ss{input};
    std::string s;
    std::vector<std::string> res;

    while (ss >> s) {
        res.push_back(s);
    }

    return res;
}

std::unique_ptr<Command> CommandParser::createCommand(const std::string& name)
{
    return registry.contains(name) ? registry[name]() : nullptr;
}

CommandParser::CommandParser()
{
    registry["pwd"]     = [] { return std::make_unique<PWDCommand>(); };
    registry["cd"]      = [] { return std::make_unique<CDCommand>(); };
    registry["mkdir"]   = [] { return std::make_unique<MKDIRCommand>(); };
    registry["ls"]      = [] { return std::make_unique<LSCommand>(); };
    registry["rmdir"]   = [] { return std::make_unique<RMDIRCommand>(); };
    registry["rm"]      = [] { return std::make_unique<RMDCommand>(); };
    registry["touch"]   = [] { return std::make_unique<TOUCHCommand>(); };
    registry["echo"]    = [] { return std::make_unique<ECHOCommand>(); };
    registry["cat"]     = [] { return std::make_unique<CATCommand>(); };
    registry["cp"]      = [] { return std::make_unique<CPCommand>(); };
    registry["mv"]      = [] { return std::make_unique<MVCommand>(); };
    registry["grep"]    = [] { return std::make_unique<GREPCommand>(); };
    registry["toJson"]  = [] { return std::make_unique<ToJsonCommand>(); };
}

// ---------------- PWDCommand ----------------
void PWDCommand::execute(FileSystemManager& fsManager, [[maybe_unused]] const std::vector<std::string>& args)
{
    std::cout << fsManager.pwd() << std::endl;
}

// ---------------- CDCommand ----------------
void CDCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    fsManager.cd(args.back());
}

// ---------------- MKDIRCommand ----------------
void MKDIRCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    fsManager.mkdir(args.back());
}

// ---------------- LSCommand ----------------
void LSCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    std::string path = args.empty() ? "" : args[0];
    auto vec = fsManager.ls(path);

    for (const std::string& s : vec) {
        std::cout << s << " ";
    }

    std::cout << std::endl;
}

// ---------------- RMDIRCommand ----------------
void RMDIRCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    std::string name = args.front();
    std::string option = args.size() == 2 ? args.back() : "";

    if (!option.empty() && option != "-r") {
        if (name == "-r") std::swap(name, option);
        else throw InvalidOptionException(option);
    }

    fsManager.rmdir(name, option == "-r");
}

// ---------------- RMDCommand ----------------
void RMDCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    fsManager.rm(args.back());
}

// ---------------- TOUCHCommand ----------------
void TOUCHCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    for (const std::string& name : args) {
        fsManager.touch(name);
    }
}

// ---------------- ECHOCommand ----------------
void ECHOCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    auto it = std::find_if(args.begin(), args.end(), [] (const std::string& arg) {
        return arg == ">" || arg == ">>";
    });

    if (it != args.end()) {
        std::ostringstream ss;
        for (auto iter{args.begin()}; iter != it; ++iter) {
            if (iter != args.begin()) ss << " ";
            ss << *iter;
        }

        std::string message{ss.str()};
        std::string op{*it};
        if (std::next(it) == args.end()) throw InvalidOperationException("No file specified for redirection");

        std::string fileName{*std::next(it)};
        fsManager.writeToFile(fileName, message, op == ">>");
    }
    else {
        std::ostringstream ss;
        for (size_t i{}; i < args.size(); ++i) {
            if (i > 0) ss << " ";
            ss << args[i];
        }
        std::cout << ss.str() << std::endl;
    }
}

// ---------------- CATCommand ----------------
void CATCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    std::cout << fsManager.readFile(args.front()) << std::endl;
}

// ---------------- CPCommand ----------------
void CPCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    if (args.size() == 3) {
        if (args[0] == "-r") {
            fsManager.cp(args[1], args[2], true);
        }
        else if (args[2] == "-r") {
            fsManager.cp(args[0], args[1], true);
        }
        else {
            throw InvalidOperationException("Invalid command for copy operation");
        }
    }
    else {
        fsManager.cp(args[0], args[1]);
    }
}

// ---------------- MVCommand ----------------
void MVCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    if (args.size() == 3) {
        if (args[0] == "-r") {
            fsManager.mv(args[1], args[2], true);
        }
        else if (args[2] == "-r") {
            fsManager.mv(args[0], args[1], true);
        }
        else {
            throw InvalidOperationException("Invalid command for move operation");
        }
    }
    else {
        fsManager.mv(args[0], args[1]);
    }
}

// ---------------- GREPCommand ----------------
void GREPCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    std::optional<std::vector<std::string>> res;
    if (args.size() == 3) {
        if (args[0] == "-r") {
            res = fsManager.grep(args[1], args[2], true);
        }
        else if (args[2] == "-r") {
            res = fsManager.grep(args[0], args[1], true);
        }
        else {
            throw InvalidOperationException("Invalid operation for grep command");
        }
    }
    else {
        res = fsManager.grep(args[0], args[1]);
    }

    if (res.has_value()) {
        for (const auto& s : res.value()) {
            std::cout << s << "\n";
        }
    }   
    else {
        std::cout << "Pattern not found\n";
    }
}

// ---------------- ToJsonCommand ----------------
void ToJsonCommand::execute(FileSystemManager& fsManager, const std::vector<std::string>& args)
{
    const std::string& path = args[0];
    const std::string& outputFile = args[2];

    FileSystemManager::json j = fsManager.convertToJson(path);

    std::ofstream out(outputFile, std::ios::trunc);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFile);
    }

    out << j.dump(4) << "\n";
    out.close();
}
