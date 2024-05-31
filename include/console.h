#ifndef RISCVDB_CONSOLE_H
#define RISCVDB_CONSOLE_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <cassert>
#include <deque>
#include <functional>
#include "simhost.h"

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


class SigIntHandler {
public:
    SigIntHandler(std::function<void(int)> handler);
    ~SigIntHandler();

private:
    const std::function<void(int)> m_handler;

    // used to handle SIGINTs (so commands can be Ctrl-C'd)
    void signalHandler(int signum); // individual signal handler
    static void static_signalHandler(int signum); // actual signal handler
    static std::deque<SigIntHandler*> s_instances;
};


class Console {
public:
    static const std::string CONSOLE_PROMPT;

    Console(SimHost& simHost);
    int run();
    int runFromScript(const std::string& scriptFilename);
    void printHelp();
    void printHelpCmd(std::string& cmdName);

private:
    void addCmd(std::shared_ptr<ConsoleCommand> cmd);
    std::shared_ptr<ConsoleCommand> findCmd(std::string& cmdName);

    std::map<std::string, std::shared_ptr<ConsoleCommand>> m_commandsLong;
    std::map<std::string, std::shared_ptr<ConsoleCommand>> m_commandsShort;

    ConsoleCommand::CmdRetType runCommand(const std::string& input);
};

} // namespace riscvdb

#endif // RISCVDB_CONSOLE_H
