#ifndef STONE_OPTION_OPTIONS_H
#define STONE_OPTION_OPTIONS_H

#include "stone/Basic/ColorStream.h"
#include "stone/Basic/FileType.h"
#include "stone/Option/ActionKind.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"

#include <memory>

namespace llvm {
namespace opt {
class OptTable;
}
} // namespace llvm

namespace stone {
class Action;

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
  CacheInvariant = (1 << 18),

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
unsigned GetArgID(const llvm::opt::Arg *arg);
llvm::StringRef GetArgName(const llvm::opt::Arg *arg);

void PrintArg(ColorStream &outStream, const char *arg, llvm::StringRef tempDir);

} // namespace opts
std::unique_ptr<llvm::opt::OptTable> CreateOptTable();

class StandardOptions {
  /// The driver options
  std::unique_ptr<llvm::opt::OptTable> optTable;

protected:
  // bool showHiddenHelp = false;

  /// The path the executing program
  // llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  // llvm::StringRef mainExecutableName;

  /// The current working director
  // llvm::StringRef workingDirectory;

  bool printLifecycle = false;

  bool printStatistics = false;

  bool printDiagnostics = true;

public:
  StandardOptions();

public:
  /// The main options table
  llvm::opt::OptTable &GetOptTable() const { return *optTable; }

  /// \check that there exist a working directory
  // bool HasWorkingDirectory() const {
  //   return !workingDirectory.empty() && workingDirectory.size() > 0;
  // }
  // /// \return working directory for the compilation
  // llvm::StringRef GetWorkingDirectory() const { return workingDirectory; }

  // /// \return the main executable path
  // llvm::StringRef GetMainExecutablePath() const { return mainExecutablePath;
  // }

  // /// \check that there exist the main executable path
  // bool HasMainExecutablePath() const {
  //   return !mainExecutablePath.empty() && mainExecutablePath.size() > 0;
  // }
  // /// \return the main executable name
  // llvm::StringRef GetMainExecutableName() const { return mainExecutableName;
  // }

  // /// \check that there exist the main executable path
  // bool HasMainExecutableName() const {
  //   return !mainExecutableName.empty() && mainExecutableName.size() > 0;
  // }
};

} // namespace stone

#endif
