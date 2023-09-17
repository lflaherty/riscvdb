#ifndef RISCVDB_CONSOLE_H
#define RISCVDB_CONSOLE_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <cassert>

namespace riscvdb {

class ConsoleCommand {
public:
    enum CmdRetType {
        CmdRetType_OK,
        CmdRetType_ERROR,
        CmdRetType_QUIT,
    };

    virtual ~ConsoleCommand() = default;

    virtual CmdRetType run(std::vector<std::string>& args) = 0;
    virtual std::string nameLong() = 0;
    virtual std::string nameShort() = 0;
    virtual std::string helpStr();
    virtual std::string extendedHelpStr();
};

class Console {
public:
    Console();
    int run();
    void printHelp();
    void printHelpCmd(std::string& cmdName);

private:
    void addCmd(std::shared_ptr<ConsoleCommand> cmd);

    std::map<std::string, std::shared_ptr<ConsoleCommand>> m_commandsLong;
    std::map<std::string, std::shared_ptr<ConsoleCommand>> m_commandsShort;
};

} // namespace riscvdb

#endif // RISCVDB_CONSOLE_H
