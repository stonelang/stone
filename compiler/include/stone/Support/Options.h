#ifndef STONE_OPTION_OPTIONS_H
#define STONE_OPTION_OPTIONS_H

#include "stone/Basic/ColorStream.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/TargetParser/Triple.h"

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
  ExcludeCompilerOption = (1 << 5),
  DriverOption = (1 << 6),
  ExcludeDriverOption = (1 << 7),
  DebugOption = (1 << 8),
  ArgumentIsPath = (1 << 9),
  ModuleInterfaceOption = (1 << 10),
  SupplementaryOutput = (1 << 11),
  StoneAPIExtractOption = (1 << 12),
  StoneSymbolGraphExtractOption = (1 << 13),
  StoneAPIDigesterOption = (1 << 14),
  ExcludeBatchOption = (1 << 15),
  DoesNotAffectIncrementalBuild = (1 << 16),
  CacheInvariant = (1 << 17),

};

enum OptID {
  OPT_INVALID = 0, // This is not an option ID.
#define OPTION(...) LLVM_MAKE_OPT_ID(__VA_ARGS__),
#include "stone/Support/Options.inc"
  OPT_LAST
#undef OPTION
};

llvm::StringRef GetEqualValueByOptID(const opts::OptID optID,
                                     const llvm::opt::InputArgList &args);
unsigned GetArgID(const llvm::opt::Arg *arg);
llvm::StringRef GetArgName(const llvm::opt::Arg *arg);

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

  /// Default target triple.
  std::string defaultTargetTriple;

  /// The current triple
  llvm::Triple currentTriple;

  /// \The working director
  llvm::SmallString<128> workingDirectory;

public:
  Options();

  Options(const Options &) = delete;
  void operator=(const Options &) = delete;
  Options(Options &&) = delete;
  void operator=(Options &&) = delete;

public:
  /// The main options table
  const llvm::opt::OptTable &GetOptTable() const { return *optTable; }
  /// The main options table
  llvm::opt::OptTable &GetOptTable() { return *optTable; }

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
  /// \The primary mode of the
  llvm::opt::Arg *GetPrimaryModeArg() const { return primaryModeArg; }

  /// \ Set the primary mode
  void SetPrimaryModeArg(llvm::opt::Arg *arg) { primaryModeArg = arg; }

  /// Return the primary mode
  unsigned GetPrimaryModeOptID() const;

  /// \return true if there exist a primary mode
  bool HasPrimaryModeArg() const { return primaryModeArg != nullptr; }

  /// \return true if there exist a working directory
  bool HasWorkingDirectory() const {
    return !workingDirectory.empty() && workingDirectory.size() > 0;
  }

  /// Set the working directory
  void SetWorkingDirectory(llvm::StringRef directory) {
    workingDirectory = directory;
  }

  /// \return the working directory of the process
  llvm::StringRef GetWorkingDirectory() const { return workingDirectory; }
  // Set the current triple
  void SetCurrentTriple(llvm::Triple triple) { currentTriple = triple; }

  /// \ Return the current triple
  const llvm::Triple &GetCurrentTriple() const { return currentTriple; }
  /// \ Return the current triple
  llvm::Triple &GetCurrentTriple() { return currentTriple; }

  /// \ Return the current triple
  bool HasCurrentTriple() const {
    return Options::HasCurrentTriple(currentTriple);
  }

public:
  /// \return true if the OS is unknown.
  bool IsUnknownOS() const;
  /// \return true if the OS is Darwin.
  bool IsDarwinOS() const;
  /// \return true if the OS is Mac.
  bool IsMacOSX() const;
  /// \return true if the OS is Linux.
  bool IsLinuxOS() const;
  /// \return true if the OS is Android.
  bool IsAndroidOS() const;
  /// \return true if the OS is FreeBSD.
  bool IsFreeBSDOS() const;
  /// \return true if the OS is OpenBSD.
  bool IsOpenBSDOS() const;
  /// \return true if the OS is Windows.
  bool IsWin32OS() const;

public:
  static bool HasCurrentTriple(llvm::Triple triple);

  /// Print the options
  void PrintHelp(bool showHidden = false);
};

} // namespace stone

#endif
