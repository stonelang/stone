#ifndef STONE_OPTION_SUPPORT_H
#define STONE_OPTION_SUPPORT_H

#include "stone/Core/Context.h"
#include "stone/Core/File.h"

#include "stone/Option/Mode.h"
#include "stone/Option/Options.h"
#include "llvm/Option/ArgList.h"

namespace stone {
namespace opts {
class Support final {

  std::unique_ptr<Mode> mode;

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask;

  unsigned missingArgIndex;
  unsigned missingArgCount;

  /// The options table
  std::unique_ptr<llvm::opt::OptTable> optst;

  /// The input argument list
  std::unique_ptr<llvm::opt::InputArgList> ial;

public:
  file::Files inputFiles;

public:
  Support();

  /// Return true means successful
  bool ParseArgs(llvm::ArrayRef<const char *> args, Context *ctx = nullptr);

public:
  llvm::opt::OptTable &GetOpts() const {
    assert(optst);
    return *optst.get();
  }

  Mode &GetMode() {
    assert(mode);
    return *mode.get();
  }

  const Mode &GetMode() const {
    assert(mode);
    return *mode.get();
  }
  llvm::opt::InputArgList &GetInputArgList() {
    assert(ial);
    return *ial.get();
  }

  void SetIncludedFlagsBitmask(unsigned flag) { includedFlagsBitmask = flag; }
  void SetExcludedFlagsBitmask(unsigned flag) { excludedFlagsBitmask = flag; }

  unsigned GetMissingArgIndex() const { return missingArgIndex; }
  unsigned GetMissingArgCount() const { return missingArgCount; }

  void PrintHelp();

  // void BuildInputFiles();
  // file::Files GetInputFiles() { return inputFiles; }
  // void AddInputFile(file::File &file);

private:
  /// Think about static
  static std::unique_ptr<Mode> CreateMode(const llvm::opt::InputArgList &ial);
};

} // namespace opts
} // namespace stone
#endif
