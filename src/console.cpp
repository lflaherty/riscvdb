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
  addCmd<CmdHelp>();
  addCmd<CmdQuit>();
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

    // special cases:
    if (tokens[0] == "help" || tokens[0] == "h") {
        parseHelp(tokens);
        continue;
    }

    // find general commands
    std::map<std::string, std::shared_ptr<ConsoleCommand>>::iterator cmdIt;
    cmdIt = m_commandsLong.find(tokens[0]);
    if (cmdIt != m_commandsLong.end()) {
        ConsoleCommand::CmdRetType ret = cmdIt->second->run();
        if (ret == ConsoleCommand::CmdRetType_QUIT) {
            break;
        }
        continue;
    }

    cmdIt = m_commandsShort.find(tokens[0]);
    if (cmdIt != m_commandsShort.end()) {
        ConsoleCommand::CmdRetType ret = cmdIt->second->run();
        if (ret == ConsoleCommand::CmdRetType_QUIT) {
            break;
        }
        continue;
    }
  }
  return 0;
}

void Console::parseHelp(std::vector<std::string>& args) {
    switch (args.size()) {
        case 1:
            printHelp();
            break;
        case 2:
            printHelpCmd(args[1]);
            break;
        default:
            std::cerr << "error: unexpected parameters" << std::endl;
    }
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

} // namespace riscvdb
