#include "console.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

// commands:
#include "commands/help.h"
#include "commands/quit.h"

namespace riscvdb {

std::string ConsoleCommand::helpStr() { return std::string(); }

std::string ConsoleCommand::extendedHelpStr() { return std::string(); }

Console::Console() {
  addCmd(std::make_shared<CmdHelp>(this));
  addCmd(std::make_shared<CmdQuit>());
}

int Console::run() {
  while (true) {
    std::cout << "> ";

    std::string input;
    std::getline(std::cin, input);

    std::istringstream buffer(input);
    std::vector<std::string> tokens{std::istream_iterator<std::string>(buffer),
                                    std::istream_iterator<std::string>()};
    
    if (tokens.size() == 0) {
        continue;
    }

    // general commands
    std::map<std::string, std::shared_ptr<ConsoleCommand>>::iterator cmdIt;
    cmdIt = m_commandsLong.find(tokens[0]);
    if (cmdIt != m_commandsLong.end()) {
        ConsoleCommand::CmdRetType ret = cmdIt->second->run(tokens);
        if (ret == ConsoleCommand::CmdRetType_QUIT) {
            break;
        }
        continue;
    }

    cmdIt = m_commandsShort.find(tokens[0]);
    if (cmdIt != m_commandsShort.end()) {
        ConsoleCommand::CmdRetType ret = cmdIt->second->run(tokens);
        if (ret == ConsoleCommand::CmdRetType_QUIT) {
            break;
        }
        continue;
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
    (void)cmdName;
    // TODO
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

} // namespace riscvdb
