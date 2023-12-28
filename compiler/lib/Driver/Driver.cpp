#include "stone/Driver/Driver.h"
#include "stone/Option/Options.h"

using namespace stone;

using namespace llvm::opt;

Driver::Driver() {}

std::unique_ptr<InputArgList>
Driver::ParseCommandLine(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::NoDriverOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  optTable = stone::CreateOptTable();
  auto inputArgList = std::make_unique<InputArgList>(
      optTable->ParseArgs(args, missingArgIndex, missingArgCount,
                          includedFlagsBitmask, excludedFlagsBitmask));

  return inputArgList;
}