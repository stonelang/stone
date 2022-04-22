#ifndef STONE_SESSION_OPTIONS_H
#define STONE_SESSION_OPTIONS_H

#include <memory>

#include "stone/Session/Mode.h"

namespace llvm {
namespace opt {
class OptTable;
}
} // namespace llvm

namespace stone {
namespace opts {
enum OptFlag {
  LangOption = (1 << 4),
  NoLangOption = (1 << 5),
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
#include "stone/Session/StoneOptions.inc"
  LAST
#undef OPTION
};

std::unique_ptr<llvm::opt::OptTable> CreateOptTable();
} // namespace opts

} // namespace stone

#endif
