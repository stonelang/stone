#ifndef STONE_OPTION_OPTIONS_H
#define STONE_OPTION_OPTIONS_H

#include "stone/Basic/ColorStream.h"
#include "stone/Support/ActionKind.h"

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

enum OptID {
  OPT_INVALID = 0, // This is not an option ID.
#define OPTION(...) LLVM_MAKE_OPT_ID(__VA_ARGS__),
#include "stone/Support/Options.inc"
  OPT_LAST
#undef OPTION
};

ActionKind GetActionKindByOptID(const unsigned optID);
llvm::StringRef GetEqualValueByOptID(const opts::OptID optID,
                                     const llvm::opt::InputArgList &args);
unsigned GetArgID(const llvm::opt::Arg *arg);
llvm::StringRef GetArgName(const llvm::opt::Arg *arg);

void PrintArg(ColorStream &outStream, const char *arg, llvm::StringRef tempDir);

} // namespace opts

class Options {
  /// Stone options
  std::unique_ptr<llvm::opt::OptTable> optTable;

protected:
  /// \Flag to control the options that are loaded
  unsigned includedFlagsBitmask = 0;
  /// \Flag to control the options that are exclued
  unsigned excludedFlagsBitmask;
  /// \Flag to indicate the argument index
  unsigned missingArgIndex;
  /// \Flag to indicate the argument count.
  unsigned missingArgCount;

  /// \The path the executing program
  llvm::StringRef mainExecutablePath;

  /// \The name of the executing program
  llvm::StringRef mainExecutableName;

  /// \The primary action arg that starts a compilation
  llvm::opt::Arg *primaryModeArg = nullptr;

public:
  Options();

  Options(const Options &) = delete;
  void operator=(const Options &) = delete;
  Options(Options &&) = delete;
  void operator=(Options &&) = delete;

public:
  /// The main options table
  const llvm::opt::OptTable &GetOptTable() const { return *optTable; }

  unsigned GetIncludedFlagsBitmask() { return includedFlagsBitmask; }
  unsigned GetExcludedFlagsBitmask() { return excludedFlagsBitmask; }
  unsigned GetMissingArgIndex() { return missingArgIndex; }
  unsigned GetMissingArgCount() { return missingArgCount; }

  /// Set the main exec path
  void SetMainExecutablePath(llvm::StringRef executablePath) {
    mainExecutablePath = executablePath;
  }
  /// \return the main executable path
  llvm::StringRef GetMainExecutablePath() const { return mainExecutablePath; }

  /// \check that there exist the main executable path
  bool HasMainExecutablePath() const {
    return !mainExecutablePath.empty() && mainExecutablePath.size() > 0;
  }

  /// Set the main exec path
  void SetMainExecutableName(llvm::StringRef executableName) {
    mainExecutableName = executableName;
  }

  /// \return the main executable name
  llvm::StringRef GetMainExecutableName() const { return mainExecutableName; }

  /// \check that there exist the main executable path
  bool HasMainExecutableName() const {
    return !mainExecutableName.empty() && mainExecutableName.size() > 0;
  }
  /// \The primary action arg that starts a compilation
  llvm::opt::Arg *GetPrimaryModeArg() { return primaryModeArg; }

  /// Print the options
  void Print(ColorStream &out) const;
};

} // namespace stone

#endif
