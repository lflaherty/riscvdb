#ifndef RISCVDB_COMMANDS_HELP_H
#define RISCVDB_COMMANDS_HELP_H

#include "console.h"
#include <string>

namespace riscvdb {

class CmdHelp : public ConsoleCommand {
public:
  ConsoleCommand::CmdRetType run();
  std::string nameLong();
  std::string nameShort();
  std::string helpStr();
};

} // namespace riscvdb

#endif // RISCVDB_COMMANDS_HELP_H
