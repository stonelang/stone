#ifndef STONE_SESSION_SESSION_H
#define STONE_SESSION_SESSION_H

#include <cassert>
#include <list>
#include <memory>
#include <string>
#include <utility>

#include "stone/Basic/Context.h"
#include "stone/Basic/File.h"
#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Basic/Timer.h"
#include "stone/Session/BaseOptions.h"
#include "stone/Session/Mode.h"
#include "stone/Session/Options.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptSpecifier.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/VirtualFileSystem.h"

namespace llvm {
class raw_fd_ostream;
class Timer;
class TimerGroup;
} // namespace llvm

namespace stone {

class Session {
protected:
  Context ctx;

  std::unique_ptr<Mode> mode;

  /// The options table
  std::unique_ptr<llvm::opt::OptTable> optst;

  /// The input argument list
  std::unique_ptr<llvm::opt::InputArgList> ial;

  /// The translated arguments.
  // TODO? std::unique_ptr<llvm::opt::DerivedArgList> dal;

  std::unique_ptr<llvm::Timer> timer;
  std::unique_ptr<llvm::TimerGroup> timerGroup;

  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> vfs;

protected:
  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask;
  unsigned missingArgIndex;
  unsigned missingArgCount;

public:
  Session();
  void CreateTimer();

public:
  virtual llvm::opt::InputArgList &ParseArgs(llvm::ArrayRef<const char *> args);
  virtual BaseOptions &GetBaseOptions() = 0;

public:
  llvm::opt::OptTable &GetOpts() const {
    assert(optst);
    return *optst.get();
  }

  Context &GetContext() { return ctx; }
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
  // llvm::opt::DerivedArgList &GetDerivedArgList() {
  //   assert(dal);
  //   return *dal.get();
  // }

  llvm::TimerGroup &GetTimerGroup() {
    assert(timerGroup);
    return *timerGroup.get();
  }
  llvm::Timer &GetTimer() {
    assert(timer);
    return *timer.get();
  }

public:
  void SetIncludedFlagsBitmask(unsigned flag) { includedFlagsBitmask = flag; }
  void SetExcludedFlagsBitmask(unsigned flag) { excludedFlagsBitmask = flag; }
  unsigned GetMissingArgIndex() const { return missingArgIndex; }
  unsigned GetMissingArgCount() const { return missingArgCount; }

  void SetVFS(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> fs) { vfs = fs; }
  llvm::vfs::FileSystem &GetVFS() const { return *vfs; }

  bool HasError() { return GetContext().GetDiagEngine().HasError(); }

public:
  void AddInputFile(llvm::StringRef name, unsigned fileID = 0);
  void AddInputFile(llvm::StringRef name, file::Type ty, unsigned fileID = 0);

public:
  Mode &ComputeMode(const llvm::opt::InputArgList &ial);
  file::Files &BuildInputFiles(const llvm::opt::InputArgList &ial);

  llvm::StringRef ComputeWorkDir(const llvm::opt::InputArgList &ial);
  stone::Result<std::string>
  GetOptEqualValue(opts::OptID optID, const llvm::opt::InputArgList &ial);

public:
  void PrintHelp(const llvm::opt::OptTable &optst);
  void PrintTimer();
  void PrintDiagnostics();
  void PrintStatistics();
};

} // namespace stone
#endif
