#include "console.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <functional>
#include <csignal>

#include "linenoise_wrapper.h"

// commands:
#include "commands/help.h"
#include "commands/load.h"
#include "commands/run.h"
#include "commands/continue.h"
#include "commands/break.h"
#include "commands/delete.h"
#include "commands/print.h"
#include "commands/info.h"
#include "commands/step.h"
#include "commands/verbose.h"
#include "commands/quit.h"

namespace riscvdb {

/*
 * *****************************************************************************
 *
 * class ConsoleCommand
 *
 * *****************************************************************************
 */
std::string ConsoleCommand::helpStr() { return std::string(); }

std::string ConsoleCommand::extendedHelpStr() { return std::string(); }


/*
 * *****************************************************************************
 *
 * class SigIntHandler
 *
 * *****************************************************************************
 */
std::deque<SigIntHandler*> SigIntHandler::s_instances;

SigIntHandler::SigIntHandler(std::function<void(int)> handler)
: m_handler(handler)
{
    // register static signal handler?
    if (s_instances.empty())
    {
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = SigIntHandler::static_signalHandler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;

        sigaction(SIGINT, &sigIntHandler, NULL);
    }

    // sign up this command for ctrl-C signal notification
    s_instances.push_back(this);
}

SigIntHandler::~SigIntHandler()
{
    // unregister when the SigInt context is destroyed

    std::deque<SigIntHandler*>::iterator it;
    for (it = s_instances.begin(); it != s_instances.end(); it++)
    {
        if (*it == this)
        {
            s_instances.erase(it);
            break;
        }
    }

    // reset signal handler if no more left
    if (s_instances.empty())
    {
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = nullptr;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;

        sigaction(SIGINT, &sigIntHandler, NULL);
    }
}

void SigIntHandler::signalHandler(int signum)
{
    m_handler(signum);
}

void SigIntHandler::static_signalHandler(int signum)
{
    // invoke every class's individual handler
    std::for_each(s_instances.begin(),
                  s_instances.end(),
                  std::bind(std::mem_fn(&SigIntHandler::signalHandler), std::placeholders::_1, signum));
}


/*
 * *****************************************************************************
 *
 * class Console
 *
 * *****************************************************************************
 */
const std::string Console::CONSOLE_PROMPT = "(riscvdb) ";

Console::Console(SimHost& simHost)
: m_sim(simHost)
{
    // Console/app commands
    addCmd(std::make_shared<CmdHelp>(*this));
    addCmd(std::make_shared<CmdVerbose>(simHost));
    addCmd(std::make_shared<CmdQuit>());

    // Binary loading commands
    addCmd(std::make_shared<CmdLoad>());

    // Machine/sim control commands
    addCmd(std::make_shared<CmdRun>(simHost));
    addCmd(std::make_shared<CmdContinue>(simHost));
    addCmd(std::make_shared<CmdStep>(simHost));

    // Memory/register commands
    addCmd(std::make_shared<CmdPrint>(simHost));
    addCmd(std::make_shared<CmdInfo>(simHost));
    // TODO: add command to set registers

    // Breakpoint commands
    addCmd(std::make_shared<CmdBreak>());
    (void)m_sim;

    using namespace linenoise_wrapper;
    Linenoise::setHintAppearance(Linenoise::COLOR_MAGENTA, false);

    std::map<std::string, std::shared_ptr<ConsoleCommand>>::iterator it;
    for (it = m_commandsLong.begin(); it != m_commandsLong.end(); it++)
    {
        std::stringstream ss;
        ss << it->second->nameLong();
        ss << " "; // add a space - easier for the user to type the next argument
        Linenoise::addCompletion(ss.str());
    }
}

int Console::run() {
    using namespace linenoise_wrapper;

    while (true) {
        std::string input;
        Linenoise::ReturnCode ret = Linenoise::next(input, CONSOLE_PROMPT);

        if (ret == Linenoise::RETURN_QUIT || ret == Linenoise::RETURN_CTRLD)
        {
            std::cout << "Quitting" << std::endl;
            break;
        }

        if (ret == Linenoise::RETURN_CTRLC)
        {
            std::cout << "^C" << std::endl;
            continue;
        }

        std::istringstream buffer(input);
        std::vector<std::string> tokens{std::istream_iterator<std::string>(buffer),
                                        std::istream_iterator<std::string>()};

        if (tokens.size() == 0) {
            continue;
        }

        // general commands
        std::shared_ptr<ConsoleCommand> cmd = findCmd(tokens[0]);
        if (cmd) {
            ConsoleCommand::CmdRetType ret = cmd->run(tokens);
            if (ret == ConsoleCommand::CmdRetType_QUIT) {
                break;
            }
        }
    }
    return 0;
}

void Console::printHelp() {
    int widthCmdLong = 10;
    int widthCmdShort = 8;
    int widthDesc = 18;

    std::map<std::string, std::shared_ptr<ConsoleCommand>>::iterator it;
    for (it = m_commandsLong.begin(); it != m_commandsLong.end(); it++) {
        widthCmdLong = std::max(widthCmdLong, static_cast<int>(it->second->nameLong().length()));
        widthDesc = std::max(widthCmdLong, static_cast<int>(it->second->helpStr().length()));
    }

    std::cout << std::setw(widthCmdLong) << std::left << "Command";
    std::cout << std::setw(widthCmdShort) << std::left << "Brief";
    std::cout << std::setw(widthDesc) << std::left << "Description";
    std::cout << std::endl;

    // general commands
    for (it = m_commandsLong.begin(); it != m_commandsLong.end(); it++) {
        std::cout << std::setw(widthCmdLong) << it->second->nameLong();
        std::cout << std::setw(widthCmdShort) << it->second->nameShort();
        std::cout << std::setw(widthDesc) << it->second->helpStr();
        std::cout << std::endl;
    }
}

void Console::printHelpCmd(std::string& cmdName) {
    std::shared_ptr<ConsoleCommand> cmd = findCmd(cmdName);
    std::cout << cmd->helpStr() << std::endl;

    std::string extendedHelp = cmd->extendedHelpStr();
    if (extendedHelp.length() > 0) {
        std::cout << extendedHelp << std::endl;
    }
}

void Console::addCmd(std::shared_ptr<ConsoleCommand> cmd) {
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

std::shared_ptr<ConsoleCommand> Console::findCmd(std::string& cmdName) {
    std::map<std::string, std::shared_ptr<ConsoleCommand>>::iterator cmdIt;
    cmdIt = m_commandsLong.find(cmdName);
    if (cmdIt != m_commandsLong.end()) {
        return cmdIt->second;
    }

    cmdIt = m_commandsShort.find(cmdName);
    if (cmdIt != m_commandsShort.end()) {
        return cmdIt->second;
    }

    std::cerr << "error: unknown command " << cmdName << std::endl;
    return nullptr;
}

} // namespace riscvdb
