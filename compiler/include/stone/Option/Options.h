#ifndef STONE_OPTION_OPTIONS_H
#define STONE_OPTION_OPTIONS_H

#include "stone/Basic/Color.h"
#include "stone/Basic/File.h"
#include "stone/Option/ActionKind.h"

#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"

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
#include "stone/Option/Options.inc"
  LAST
#undef OPTION
};

ActionKind GetActionKindByOptionID(const unsigned actionOptionID);
llvm::StringRef GetEqualValueByOptionID(const opts::OptID optID,
                                        const llvm::opt::InputArgList &args);
unsigned GetArgID(llvm::opt::Arg *arg);
llvm::StringRef GetArgName(llvm::opt::Arg *arg);
void PrintArg(ColorStream &outStream, const char *arg, llvm::StringRef tempDir);

} // namespace opts
std::unique_ptr<llvm::opt::OptTable> CreateOptTable();

class InputFile final {

  file::FileType fileType;
  const llvm::opt::Arg *input;

public:
  InputFile(file::FileType fileType, llvm::opt::Arg *input)
      : fileType(fileType), input(input) {}

  file::FileType GetType() { return fileType; }
  const llvm::opt::Arg *GetInput() { return input; }
};

/// Type used for a list of input arguments.
using InputFileList = llvm::SmallVector<InputFile, 16>;

} // namespace stone

#endif
