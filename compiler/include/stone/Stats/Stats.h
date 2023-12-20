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
class CompilerStatsTracer;
class SourceFile;
class SrcMgr;
class Stmt;
class Type;
class StatsTracer;
class StatsReporter;
class Compiler;

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

// To trace an entity, you have to provide a StatsTraceFormatter for it. This is
// a separate type since we do not have retroactive conformances in C++, and it
// is a type that takes void* arguments since we do not have existentials
// separate from objects in C++. Pity us.
struct StatsTraceFormatter {
  virtual void TraceName(const void *Entity, raw_ostream &OS) const = 0;

  virtual void TraceLoc(const void *Entity, SrcMgr *SourceMgr,
                        clang::SourceManager *ClangSourceMgr,
                        raw_ostream &OS) const = 0;
  virtual ~StatsTraceFormatter();
};

class StatsEvent {
public:
  uint64_t TimeUSec;
  uint64_t LiveUSec;
  bool IsEntry;
  llvm::StringRef statName;
  llvm::StringRef CounterName;
  int64_t CounterDelta;
  int64_t CounterValue;
  const void *Entity;
  const StatsTraceFormatter *Formatter;
};

class StatsTracer {
public:
  llvm::TimeRecord savedTime;
  llvm::StringRef eventName;
  const void *entity;
  const StatsTraceFormatter *formatter;
  StatsReporter *reporter;

public:
  StatsTracer(StatsReporter *reporter, StringRef eventName, const void *entity,
              const StatsTraceFormatter *formatter);

public:
  // In the general case we do not know how to format an entity for tracing.
  template <typename T>
  static const StatsTraceFormatter *GetStatsTraceFormatter() {
    return nullptr;
  }
};

class StatsReporter {

protected:
  // We only write fine-grained trace entries when the user passed
  // -trace-stats-events, but we recycle the same CompilerStatsTracers to give
  // us some free recursion-save phase timings whenever -trace-stats-dir is
  // active at all. Reduces redundant machinery.
  class RecursionSafeTimers;

  // We also keep a few banks of optional hierarchical profilers for times and
  // statistics, activated with -profile-stats-events and
  // -profile-stats-entities, which are part way between the detail level of the
  // aggregate statistic JSON files and the fine-grained CSV traces. Naturally
  // these are written in yet a different file format: the input format for
  // flamegraphs.
  struct StatsProfiler;

protected:
  bool currentProcessExitStatusSet;
  int currentProcessExitStatus;

  long maxChildRSS = 0;
  llvm::SmallString<128> StatsFilename;
  llvm::SmallString<128> TraceFilename;
  llvm::SmallString<128> ProfileDirname;
  llvm::TimeRecord StartedTime;
  std::thread::id MainThreadID;

  // This is unique_ptr because NamedRegionTimer is non-copy-constructable.
  std::unique_ptr<llvm::NamedRegionTimer> Timer;

  SrcMgr *SourceMgr;
  clang::SourceManager *ClangSourceMgr;

  llvm::Optional<std::vector<StatsEvent>> statsEvents;

  // These are unique_ptr so we can use incomplete types here.
  std::unique_ptr<RecursionSafeTimers> RecursiveTimers;

  /// Profilers
  std::unique_ptr<StatsProfiler> EventProfilers;
  std::unique_ptr<StatsProfiler> EntityProfilers;

  /// Whether we are currently flushing statistics and should not therefore
  /// record any additional stats until we've finished.
  bool IsFlushingTracesAndProfiles;

  void PublishAlwaysOnStatsToLLVM();
  void PrintAlwaysOnStatsAndTimers(raw_ostream &OS);

  StatsReporter(llvm::StringRef ProgramName, llvm::StringRef AuxName,
                llvm::StringRef Directory, SrcMgr *SM,
                clang::SourceManager *CSM, bool TraceEvents, bool ProfileEvents,
                bool ProfileEntities);

public:
  StatsReporter(llvm::StringRef ProgramName, llvm::StringRef ModuleName,
                llvm::StringRef InputName, llvm::StringRef TripleName,
                llvm::StringRef OutputType, llvm::StringRef OptType,
                llvm::StringRef Directory, SrcMgr *SM = nullptr,
                clang::SourceManager *CSM = nullptr, bool TraceEvents = false,
                bool ProfileEvents = false, bool ProfileEntities = false);
  ~StatsReporter();

  void FlushTracesAndProfiles();
  void NoteCurrentProcessExitStatus(int);
  void SaveStats(StatsTracer const &T, bool IsEntry);
  void RecordJobMaxRSS(long rss);
  int64_t GetChildrenMaxResidentSetSize();
};

class DriverStatsReporter final : public StatsReporter {

public:
  struct DriverCounters {
#define DRIVER_STAT(ID) int64_t ID;
#include "Stats.def"
#undef DRIVER_STAT
  };

  llvm::Optional<DriverCounters> driverCounters;

public:
  DriverStatsReporter(llvm::StringRef ModuleName, llvm::StringRef InputName,
                      llvm::StringRef TripleName, llvm::StringRef OutputType,
                      llvm::StringRef OptType, llvm::StringRef Directory,
                      SrcMgr *SM = nullptr, clang::SourceManager *CSM = nullptr,
                      bool TraceEvents = false, bool ProfileEvents = false,
                      bool ProfileEntities = false);
  ~DriverStatsReporter();

  DriverCounters &GetCounters();
};

struct CompilerStatsFormatter final : public StatsTraceFormatter {
  void TraceName(const void *Entity, raw_ostream &OS) const override {}

  void TraceLoc(const void *Entity, SrcMgr *SourceMgr,
                clang::SourceManager *ClangSourceMgr,
                raw_ostream &OS) const override {}
};

class CompilerStatsReporter final : public StatsReporter {

public:
  struct CompilerCounters {
#define COMPILER_STAT(NAME, ID) int64_t ID;
#include "Stats.def"
#undef COMPILER_STAT
  };

  /// Counters that are always on.
  llvm::Optional<CompilerCounters> compilerCounters;
  llvm::Optional<CompilerCounters> lastTracedCompilerCounters;

public:
  CompilerStatsReporter(llvm::StringRef ModuleName, llvm::StringRef InputName,
                        llvm::StringRef TripleName, llvm::StringRef OutputType,
                        llvm::StringRef OptType, llvm::StringRef Directory,
                        SrcMgr *SM = nullptr,
                        clang::SourceManager *CSM = nullptr,
                        bool TraceEvents = false, bool ProfileEvents = false,
                        bool ProfileEntities = false);
  ~CompilerStatsReporter();

public:
  CompilerCounters &GetCounters();
  void CountASTStats(Compiler &compiler);
  void CountDeclStats(Compiler &compiler);
  void CountExprStats(Compiler &compiler);
  void CountTypeStats(Compiler &compiler);
  void CountSourceFileStats(Compiler &compiler);
};

// This is a non-nested type just to make it less work to write at call sites.
class CompilerStatsTracer final : public StatsTracer {

private:
  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName, const void *Entity,
                      const StatsTraceFormatter *Formatter);

public:
  CompilerStatsReporter *statsReporter;

  CompilerStatsTracer();
  CompilerStatsTracer(CompilerStatsTracer &&other);
  CompilerStatsTracer &operator=(CompilerStatsTracer &&);
  ~CompilerStatsTracer();
  CompilerStatsTracer(const CompilerStatsTracer &) = delete;
  CompilerStatsTracer &operator=(const CompilerStatsTracer &) = delete;

  /// These are the convenience constructors you want to be calling throughout
  /// the compiler: they select an appropriate trace formatter for the provided
  /// entity type, and produce a tracer that's either active or inert depending
  /// on whether the provided \p statsReporter is null (nullptr means "tracing
  /// is disabled").
  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName);

  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName, const Decl *D);

  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName, const clang::Decl *D);

  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName, const Expr *E);

  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName, const SourceFile *F);

  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName, const Stmt *S);

  CompilerStatsTracer(CompilerStatsReporter *statsReporter,
                      llvm::StringRef statName, const Type *ty);
};

} // namespace stone

#endif
