#ifndef STONE_OPTION_OPTUTIL_H
#define STONE_OPTION_OPTUTIL_H

#include "stone/Core/Context.h"
#include "stone/Core/File.h"

#include "stone/Option/BaseOptions.h"
#include "stone/Option/Mode.h"
#include "stone/Option/Options.h"
#include "llvm/Option/ArgList.h"

namespace stone {
namespace opts {

class OptInvocation {

  std::unique_ptr<Mode> mode;

  /// The options table
  std::unique_ptr<llvm::opt::OptTable> optst;

  /// The input argument list
  std::unique_ptr<llvm::opt::InputArgList> ial;

protected:
  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask;
  unsigned missingArgIndex;
  unsigned missingArgCount;

public:
  OptInvocation();

public:
  virtual llvm::opt::InputArgList &ParseArgs(llvm::ArrayRef<const char *> args);

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

public:
  virtual Context &GetContext() = 0;
  virtual BaseOptions &GetBaseOptions() = 0;

public:
  void PrintHelp();

  void AddInputFile(llvm::StringRef name);
  void AddInputFile(llvm::StringRef name, file::Type ty);

public:
  Mode &CreateMode(const llvm::opt::InputArgList &ial);
  file::Files &BuildInputFiles(const llvm::opt::InputArgList &ial);
};

} // namespace opts
} // namespace stone
#endif
