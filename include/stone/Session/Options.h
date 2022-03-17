#ifndef STONE_OPTION_OPTIONS_H
#define STONE_OPTION_OPTIONS_H

#include "stone/Session/Mode.h"

#include <memory>

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
