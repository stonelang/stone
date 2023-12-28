#include "stone/Driver/Driver.h"
#include "stone/Option/Options.h"

using namespace stone;

using namespace llvm::opt;

Driver::Driver()
    : fileMgr(GetFileSystemOptions()), optTable(stone::CreateOptTable()) {}

std::unique_ptr<InputArgList>
Driver::ParseCommandLine(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::NoDriverOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  auto inputArgList = std::make_unique<InputArgList>(
      GetOptTable().ParseArgs(args, missingArgIndex, missingArgCount,
                              includedFlagsBitmask, excludedFlagsBitmask));

  return inputArgList;
}