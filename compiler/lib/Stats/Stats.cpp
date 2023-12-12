#include "stone/Stats/Stats.h"

#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Config/config.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/SaveAndRestore.h"
#include "llvm/Support/raw_ostream.h"

#include <chrono>
#include <limits>
#include <memory>

using namespace stone;

class StatsReporter::RecursionSafeTimers {
  struct RecursionSafeTimer {
    llvm::Optional<llvm::NamedRegionTimer> Timer;
    size_t RecursionDepth;
  };

  llvm::StringMap<RecursionSafeTimer> Timers;

public:
  void BeginTimer(StringRef Name) {
    RecursionSafeTimer &T = Timers[Name];
    if (T.RecursionDepth == 0) {
      T.Timer.emplace(Name, Name, "stone", "Stone compilation");
    }
    ++T.RecursionDepth;
  }

  void EndTimer(StringRef Name) {
    auto I = Timers.find(Name);
    assert(I != Timers.end());
    RecursionSafeTimer &T = I->getValue();
    assert(T.RecursionDepth != 0);
    --T.RecursionDepth;
    if (T.RecursionDepth == 0) {
      T.Timer.reset();
    }
  }
};

class StatsReporter::StatsProfiler {};

static std::string CleanName(StringRef n) {
  std::string tmp;
  for (auto c : n) {
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
        ('0' <= c && c <= '9') || (c == '.'))
      tmp += c;
    else
      tmp += '_';
  }
  return tmp;
}
static std::string AuxName(StringRef ModuleName, StringRef InputName,
                           StringRef TripleName, StringRef OutputType,
                           StringRef OptType) {
  if (InputName.empty()) {
    InputName = "all";
  }
  // Dispose of path prefix, which might make composite name too long.
  InputName = llvm::sys::path::filename(InputName);
  if (OptType.empty()) {
    OptType = "Onone";
  }
  if (!OutputType.empty() && OutputType.front() == '.') {
    OutputType = OutputType.substr(1);
  }
  if (!OptType.empty() && OptType.front() == '-') {
    OptType = OptType.substr(1);
  }
  return (CleanName(ModuleName) + "-" + CleanName(InputName) + "-" +
          CleanName(TripleName) + "-" + CleanName(OutputType) + "-" +
          CleanName(OptType));
}

StatsReporter::StatsReporter(
    llvm::StringRef ProgramName, llvm::StringRef ModuleName,
    llvm::StringRef InputName, llvm::StringRef TripleName,
    llvm::StringRef OutputType, llvm::StringRef OptType,
    llvm::StringRef Directory, SrcMgr *SM, clang::SourceManager *CSM,
    bool TraceEvents, bool ProfileEvents, bool ProfileEntities)
    : StatsReporter(
          ProgramName,
          AuxName(ModuleName, InputName, TripleName, OutputType, OptType),
          Directory, SM, CSM, TraceEvents, ProfileEvents, ProfileEntities) {}

StatsReporter::StatsReporter(llvm::StringRef ProgramName,
                             llvm::StringRef AuxName, llvm::StringRef Directory,
                             SrcMgr *SM, clang::SourceManager *CSM,
                             bool TraceEvents, bool ProfileEvents,
                             bool ProfileEntities)
    : currentProcessExitStatusSet(false),
      currentProcessExitStatus(EXIT_FAILURE), StatsFilename(Directory),
      TraceFilename(Directory), ProfileDirname(Directory),
      StartedTime(llvm::TimeRecord::getCurrentTime()),
      MainThreadID(std::this_thread::get_id()),
      Timer(std::make_unique<llvm::NamedRegionTimer>(
          AuxName, "Building Target", ProgramName, "Running Program")),
      SourceMgr(SM), ClangSourceMgr(CSM),
      RecursiveTimers(std::make_unique<RecursionSafeTimers>()),
      IsFlushingTracesAndProfiles(false) {

  // path::append(StatsFilename, makeStatsFileName(ProgramName, AuxName));
  // path::append(TraceFilename, makeTraceFileName(ProgramName, AuxName));
  // path::append(ProfileDirname, makeProfileDirName(ProgramName, AuxName));

  // EnableStatistics(/*PrintOnExit=*/false);

  // if (TraceEvents || ProfileEvents || ProfileEntities)
  //   LastTracedFrontendCounters.emplace();
  // if (TraceEvents)
  //   FrontendStatsEvents.emplace();
  // if (ProfileEvents)
  //   EventProfilers = std::make_unique<StatsProfilers>();
  // if (ProfileEntities)
  //   EntityProfilers = std::make_unique<StatsProfilers>();
}

CompilerStatsReporter::CompilerStatsReporter(
    llvm::StringRef ModuleName, llvm::StringRef InputName,
    llvm::StringRef TripleName, llvm::StringRef OutputType,
    llvm::StringRef OptType, llvm::StringRef Directory, SrcMgr *SM,
    clang::SourceManager *CSM, bool TraceEvents, bool ProfileEvents,
    bool ProfileEntities)
    : StatsReporter("stone-compile", ModuleName, InputName, TripleName,
                    OutputType, OptType, Directory, SM, CSM, TraceEvents,
                    ProfileEvents, ProfileEntities) {}

CompilerStatsTracer::~CompilerStatsTracer() {}
