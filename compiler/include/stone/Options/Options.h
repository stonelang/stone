#ifndef STONE_OPTIONS_OPTIONS_H
#define STONE_OPTIONS_OPTIONS_H

#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"

#include <memory>

namespace llvm {
namespace opt {
class OptTable;
}
} // namespace llvm

namespace stone {
namespace opts {
enum OptFlag {
  CompilerOption = (1 << 4),
  NoCompilerOption = (1 << 5),
  DriverOption = (1 << 6),
  NoDriverOption = (1 << 7),
  DebugOption = (1 << 8),
  ArgumentIsPath = (1 << 9),
  ModuleInterfaceOption = (1 << 10),
  SupplementaryOutput = (1 << 11),
  StoneAPIExtractOption = (1 << 12),
  StoneSymbolGraphExtractOption = (1 << 13),
  StoneAPIDigesterOption = (1 << 14),
  NoBatchOption = (1 << 15),
  DoesNotAffectIncrementalBuild = (1 << 16),
  NoInteractiveOption = (1 << 17),

};

enum OptID : unsigned {
  INVALID = 0, // This is not an option ID.
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM,  \
               HELPTEXT, METAVAR, VALUES)                                      \
  ID,
#include "stone/Options/StoneOpts.inc"
  LAST
#undef OPTION
};

struct OptParsingFlags {
  unsigned includedFlagsBitmask;
  unsigned excludedFlagsBitmask;
  unsigned missingArgIndex;
  unsigned missingArgCount;
  OptParsingFlags(unsigned includedFlagsBitmask, unsigned excludedFlagsBitmask,
                  unsigned missingArgIndex, unsigned missingArgCount)
      : includedFlagsBitmask(includedFlagsBitmask),
        excludedFlagsBitmask(excludedFlagsBitmask),
        missingArgIndex(missingArgIndex), missingArgCount(missingArgCount) {}
};

using OptTableAndInputArgListPair =
    std::pair<std::unique_ptr<llvm::opt::OptTable>,
              std::unique_ptr<llvm::opt::InputArgList>>;

std::unique_ptr<llvm::opt::OptTable> CreateOptTable();
OptTableAndInputArgListPair ParseArgs(llvm::ArrayRef<const char *> args,
                                      OptParsingFlags flags);
} // namespace opts
} // namespace stone

#endif
