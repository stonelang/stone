#ifndef STONE_STATS_STATS_H
#define STONE_STATS_STATS_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcMgr.h"

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Timer.h"

#include "clang/Basic/SourceManager.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"

#include <iostream>
#include <thread>

namespace clang {
class Decl;
class SourceManager;
} // namespace clang
namespace stone {

class Decl;
class Expr;
class CompilerStatTracer;
class SourceFile;
class SrcMgr;
class Stmt;
class Type;
class StatTracer;

/// Get the number of instructions executed since this process was launched.
/// Returns 0 if the number of instructions executed could not be determined.
uint64_t GetInstructionsExecuted();

// There are a handful of cases where the swift compiler can introduce
// counter-measurement noise via nondeterminism, especially via
// parallelism; inhibiting all such cases reliably using existing avenues
// is a bit tricky and depends both on delicate build-setting management
// and some build-system support that is still pending (see
// rdar://39528362); in the meantime we support an environment variable
// ourselves to request blanket suppression of parallelism (and anything
// else nondeterministic we find).
bool EnvironmentVariableRequestedMaximumDeterminism();

// To trace an entity, you have to provide a TraceFormatter for it. This is a
// separate type since we do not have retroactive conformances in C++, and it
// is a type that takes void* arguments since we do not have existentials
// separate from objects in C++. Pity us.
struct TraceFormatter {
  virtual void TraceName(const void *Entity, raw_ostream &OS) const = 0;

  virtual void TraceLoc(const void *Entity, SrcMgr *SourceMgr,
                        clang::SourceManager *ClangSourceMgr,
                        raw_ostream &OS) const = 0;
  virtual ~TraceFormatter();
};

struct Stat {
  uint64_t TimeUSec;
  uint64_t LiveUSec;
  bool IsEntry;
  llvm::StringRef statName;
  llvm::StringRef CounterName;
  int64_t CounterDelta;
  int64_t CounterValue;
  const void *Entity;
  const TraceFormatter *Formatter;
};

class StatTracer {
public:
  llvm::TimeRecord SavedTime;
  llvm::StringRef statName;
  const void *Entity;
  const TraceFormatter *Formatter;

public:
  // In the general case we do not know how to format an entity for tracing.
  template <typename T> static const TraceFormatter *GetTraceFormatter() {
    return nullptr;
  }
};

class StatReporter {

protected:
  // We only write fine-grained trace entries when the user passed
  // -trace-stats-events, but we recycle the same CompilerStatTracers to give
  // us some free recursion-save phase timings whenever -trace-stats-dir is
  // active at all. Reduces redundant machinery.
  class RecursionSafeTimers;

  // We also keep a few banks of optional hierarchical profilers for times and
  // statistics, activated with -profile-stats-events and
  // -profile-stats-entities, which are part way between the detail level of the
  // aggregate statistic JSON files and the fine-grained CSV traces. Naturally
  // these are written in yet a different file format: the input format for
  // flamegraphs.
  struct StatProfiler;

protected:
  bool currentProcessExitStatusSet;
  int currentProcessExitStatus;

  long maxChildRSS = 0;
  llvm::SmallString<128> StatFilename;
  llvm::SmallString<128> TraceFilename;
  llvm::SmallString<128> ProfileDirname;
  llvm::TimeRecord StartedTime;
  std::thread::id MainThreadID;

  // This is unique_ptr because NamedRegionTimer is non-copy-constructable.
  std::unique_ptr<llvm::NamedRegionTimer> Timer;

  SrcMgr *SourceMgr;
  clang::SourceManager *ClangSourceMgr;

  llvm::Optional<std::vector<Stat>> stats;

  // These are unique_ptr so we can use incomplete types here.
  std::unique_ptr<RecursionSafeTimers> RecursiveTimers;

  /// Profilers
  std::unique_ptr<StatProfiler> EventProfilers;
  std::unique_ptr<StatProfiler> EntityProfilers;

  /// Whether we are currently flushing statistics and should not therefore
  /// record any additional stats until we've finished.
  bool IsFlushingTracesAndProfiles;

  void PublishAlwaysOnStatsToLLVM();
  void PrintAlwaysOnStatsAndTimers(raw_ostream &OS);

  StatReporter(llvm::StringRef ProgramName, llvm::StringRef AuxName,
               llvm::StringRef Directory, SrcMgr *SM, clang::SourceManager *CSM,
               bool TraceEvents, bool ProfileEvents, bool ProfileEntities);

public:
  StatReporter(llvm::StringRef ProgramName, llvm::StringRef ModuleName,
               llvm::StringRef InputName, llvm::StringRef TripleName,
               llvm::StringRef OutputType, llvm::StringRef OptType,
               llvm::StringRef Directory, SrcMgr *SM = nullptr,
               clang::SourceManager *CSM = nullptr, bool TraceEvents = false,
               bool ProfileEvents = false, bool ProfileEntities = false);
  ~StatReporter();

  void flushTracesAndProfiles();
  void noteCurrentProcessExitStatus(int);
  void SaveStat(StatTracer const &T, bool IsEntry);
  void recordJobMaxRSS(long rss);
  int64_t getChildrenMaxResidentSetSize();
};

class DriverStatReporter final : public StatReporter {

public:
  struct DriverCounters {
#define DRIVER_STAT(ID) int64_t ID;
#include "Stats.def"
#undef DRIVER_STAT
  };

  llvm::Optional<DriverCounters> driverCounters;

  DriverStatReporter(llvm::StringRef AuxName, llvm::StringRef Directory,
                     SrcMgr *SM, clang::SourceManager *CSM, bool TraceEvents,
                     bool ProfileEvents, bool ProfileEntities);

public:
  DriverStatReporter(llvm::StringRef ModuleName, llvm::StringRef InputName,
                     llvm::StringRef TripleName, llvm::StringRef OutputType,
                     llvm::StringRef OptType, llvm::StringRef Directory,
                     SrcMgr *SM = nullptr, clang::SourceManager *CSM = nullptr,
                     bool TraceEvents = false, bool ProfileEvents = false,
                     bool ProfileEntities = false);
  ~DriverStatReporter();

  DriverCounters &GetDriverCounters();
};

struct CompilerStatFormatter final : public TraceFormatter {
  void TraceName(const void *Entity, raw_ostream &OS) const override {}

  void TraceLoc(const void *Entity, SrcMgr *SourceMgr,
                clang::SourceManager *ClangSourceMgr,
                raw_ostream &OS) const override {}
};

class CompilerStatReporter final : public StatReporter {

public:
  struct CompilerCounters {
#define COMPILER_STAT(NAME, ID) int64_t ID;
#include "Stats.def"
#undef COMPILER_STAT
  };

  CompilerStatReporter(llvm::StringRef AuxName, llvm::StringRef Directory,
                       SrcMgr *SM, clang::SourceManager *CSM, bool TraceEvents,
                       bool ProfileEvents, bool ProfileEntities);

  /// Counters that are always on.
  llvm::Optional<CompilerCounters> compilerCounters;
  llvm::Optional<CompilerCounters> lastTracedCompilerCounters;

public:
  CompilerStatReporter(llvm::StringRef ModuleName, llvm::StringRef InputName,
                       llvm::StringRef TripleName, llvm::StringRef OutputType,
                       llvm::StringRef OptType, llvm::StringRef Directory,
                       SrcMgr *SM = nullptr,
                       clang::SourceManager *CSM = nullptr,
                       bool TraceEvents = false, bool ProfileEvents = false,
                       bool ProfileEntities = false);
  ~CompilerStatReporter();

  CompilerCounters &GetCompilerCounters();
};

// This is a non-nested type just to make it less work to write at call sites.
class CompilerStatTracer final : public StatTracer {

private:
  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName, const void *Entity,
                     const TraceFormatter *Formatter);

public:
  CompilerStatReporter *statReporter;

  CompilerStatTracer();
  CompilerStatTracer(CompilerStatTracer &&other);
  CompilerStatTracer &operator=(CompilerStatTracer &&);
  ~CompilerStatTracer();
  CompilerStatTracer(const CompilerStatTracer &) = delete;
  CompilerStatTracer &operator=(const CompilerStatTracer &) = delete;

  /// These are the convenience constructors you want to be calling throughout
  /// the compiler: they select an appropriate trace formatter for the provided
  /// entity type, and produce a tracer that's either active or inert depending
  /// on whether the provided \p statReporter is null (nullptr means "tracing is
  /// disabled").
  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName);

  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName, const Decl *D);

  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName, const clang::Decl *D);

  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName, const Expr *E);

  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName, const SourceFile *F);

  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName, const Stmt *S);

  CompilerStatTracer(CompilerStatReporter *statReporter,
                     llvm::StringRef statName, const Type *ty);
};

} // namespace stone

#endif
