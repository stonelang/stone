#ifndef STONE_SESSION_SESSION_H
#define STONE_SESSION_SESSION_H

#include <cassert>
#include <list>
#include <memory>
#include <string>
#include <utility>

#include "stone/Basic/File.h"
#include "stone/Basic/Timer.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Public.h"
#include "stone/Session/Mode.h"
#include "stone/Session/Options.h"
#include "stone/Session/SessionOptions.h"

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
#include "llvm/MC/TargetRegistry.h"
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
#include "llvm/Support/Timer.h"
#include "llvm/Support/VirtualFileSystem.h"

namespace llvm {
class raw_fd_ostream;
class Timer;
class TimerGroup;
} // namespace llvm

namespace stone {

class Session {

  llvm::StringRef programName;
  llvm::StringRef programPath;

protected:
  LangContext ctx;
  std::unique_ptr<llvm::opt::OptTable> optst;
  std::unique_ptr<llvm::Timer> timer;
  std::unique_ptr<llvm::TimerGroup> timerGroup;

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask;
  unsigned missingArgIndex;
  unsigned missingArgCount;

public:
  Session(llvm::StringRef programName, llvm::StringRef programPath);
  ~Session();

private:
  void CreateTimer();

public:
  std::unique_ptr<llvm::opt::InputArgList>
  ParseArgs(llvm::ArrayRef<const char *> args);
  virtual stone::Error ComputeOptions(llvm::opt::InputArgList &args) = 0;

  virtual void Finish() { assert(false && "Cannot call directly"); }

public:
  llvm::opt::OptTable &GetOpts() const { return *optst.get(); }
  LangContext &GetLangContext() { return ctx; }
  llvm::TimerGroup &GetTimerGroup() { return *timerGroup.get(); }
  llvm::Timer &GetTimer() { return *timer.get(); }
  llvm::StringRef GetProgramName() const { return programName; }
  llvm::StringRef GetProgramPath() const { return programPath; }
  bool HasError() { return GetLangContext().GetDiags().HasError(); }

public:
  llvm::StringRef ComputeWorkDir(const llvm::opt::InputArgList &ial);
  stone::Result<std::string>
  GetOptEqualValue(opts::OptID optID, const llvm::opt::InputArgList &ial);

public:
  void PrintHelp(raw_ostream &stream, const llvm::opt::OptTable &opts);
  void PrintArg(raw_ostream &stream, const char *arg, llvm::StringRef tempDir);
  void PrintVersion();
  void PrintTimer();
  void PrintDiagnostics();
  void PrintStatistics();
};

} // namespace stone
#endif
