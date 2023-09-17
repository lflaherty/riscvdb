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

    virtual CmdRetType run() = 0;
    virtual std::string nameLong() = 0;
    virtual std::string nameShort() = 0;
    virtual std::string helpStr();
    virtual std::string extendedHelpStr();
};

class Console {
public:
    Console();
    int run();

private:
    template <typename T> void addCmd();
    void parseHelp(std::vector<std::string>& args);
    void printHelp();
    void printHelpCmd(std::string& cmdName);

    std::map<std::string, std::shared_ptr<ConsoleCommand>> m_commandsLong;
    std::map<std::string, std::shared_ptr<ConsoleCommand>> m_commandsShort;
};

template <typename T> void Console::addCmd() {
    std::shared_ptr<ConsoleCommand> cmd = std::make_unique<T>();
    std::string nameLong = cmd->nameLong();
    std::string nameShort = cmd->nameShort();

    // make sure the command is not already registered
    assert(m_commandsLong.find(nameLong) == m_commandsLong.end());
    assert(m_commandsLong.find(nameShort) == m_commandsLong.end());
    assert(m_commandsShort.find(nameLong) == m_commandsShort.end());
    assert(m_commandsShort.find(nameShort) == m_commandsShort.end());
    
    m_commandsLong[nameLong] = cmd;
    m_commandsShort[nameShort] = cmd;
}

} // namespace riscvdb

#endif // RISCVDB_CONSOLE_H
