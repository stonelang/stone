#ifndef STONE_STATS_STATS_H
#define STONE_STATS_STATS_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/ColorStream.h"
#include "stone/Basic/List.h"

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
class SourceFile;
class SrcMgr;
class Stmt;
class Type;
class FrontendStatsTracer;

/// Get the number of instructions executed since this process was launched.
/// Returns 0 if the number of instructions executed could not be determined.
uint64_t getInstructionsExecuted();

// There are a handful of cases where the swift compiler can introduce
// counter-measurement noise via nondeterminism, especially via
// parallelism; inhibiting all such cases reliably using existing avenues
// is a bit tricky and depends both on delicate build-setting management
// and some build-system support that is still pending (see
// rdar://39528362); in the meantime we support an environment variable
// ourselves to request blanket suppression of parallelism (and anything
// else nondeterministic we find).
bool environmentVariableRequestedMaximumDeterminism();

class StatsReporter final {

public:
  struct AlwaysOnDriverCounters {
#define DRIVER_STATISTIC(ID) int64_t ID;
#include "stone/Support/Statistics.def"
#undef DRIVER_STATISTIC
  };

  struct AlwaysOnFrontendCounters {
#define FRONTEND_STATISTIC(NAME, ID) int64_t ID;
#include "stone/Support/Statistics.def"
#undef FRONTEND_STATISTIC
  };

  // To trace an entity, you have to provide a TraceFormatter for it. This is a
  // separate type since we do not have retroactive conformances in C++, and it
  // is a type that takes void* arguments since we do not have existentials
  // separate from objects in C++. Pity us.
  struct TraceFormatter {
    virtual void traceName(const void *Entity, raw_ostream &OS) const = 0;
    virtual void traceLoc(const void *Entity, SrcMgr *SourceMgr,
                          clang::SourceManager *ClangSourceMgr,
                          raw_ostream &OS) const = 0;
    virtual ~TraceFormatter();
  };

  struct FrontendStatsEvent final {
    uint64_t TimeUSec;
    uint64_t LiveUSec;
    bool IsEntry;
    llvm::StringRef EventName;
    llvm::StringRef CounterName;
    int64_t CounterDelta;
    int64_t CounterValue;
    const void *Entity;
    const TraceFormatter *Formatter;
  };

  // We only write fine-grained trace entries when the user passed
  // -trace-stats-events, but we recycle the same FrontendStatsTracers to give
  // us some free recursion-save phase timings whenever -trace-stats-dir is
  // active at all. Reduces redundant machinery.
  class RecursionSafeTimers;

  // We also keep a few banks of optional hierarchical profilers for times and
  // statistics, activated with -profile-stats-events and
  // -profile-stats-entities, which are part way between the detail level of the
  // aggregate statistic JSON files and the fine-grained CSV traces. Naturally
  // these are written in yet a different file format: the input format for
  // flamegraphs.
  struct StatsProfilers;

private:
  bool currentProcessExitStatusSet;
  int currentProcessExitStatus;
  long maxChildRSS = 0;
  SmallString<128> StatsFilename;
  SmallString<128> TraceFilename;
  SmallString<128> ProfileDirname;
  llvm::TimeRecord StartedTime;
  std::thread::id MainThreadID;

  // This is unique_ptr because NamedRegionTimer is non-copy-constructable.
  std::unique_ptr<llvm::NamedRegionTimer> Timer;

  SrcMgr *SourceMgr;
  clang::SourceManager *ClangSourceMgr;
  std::optional<AlwaysOnDriverCounters> DriverCounters;
  std::optional<AlwaysOnFrontendCounters> FrontendCounters;
  std::optional<AlwaysOnFrontendCounters> LastTracedFrontendCounters;
  std::optional<std::vector<FrontendStatsEvent>> FrontendStatsEvents;

  // These are unique_ptr so we can use incomplete types here.
  std::unique_ptr<RecursionSafeTimers> RecursiveTimers;
  std::unique_ptr<StatsProfilers> EventProfilers;
  std::unique_ptr<StatsProfilers> EntityProfilers;

  /// Whether we are currently flushing statistics and should not therefore
  /// record any additional stats until we've finished.
  bool IsFlushingTracesAndProfiles;

  void publishAlwaysOnStatsToLLVM();
  void printAlwaysOnStatsAndTimers(raw_ostream &OS);

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

  AlwaysOnDriverCounters &getDriverCounters();
  AlwaysOnFrontendCounters &getFrontendCounters();
  void flushTracesAndProfiles();
  void noteCurrentProcessExitStatus(int);
  void saveAnyFrontendStatsEvents(FrontendStatsTracer const &T, bool IsEntry);
  void recordJobMaxRSS(long rss);
  int64_t getChildrenMaxResidentSetSize();
};

// This is a non-nested type just to make it less work to write at call sites.
class FrontendStatsTracer {
  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName,
                      const void *Entity,
                      const StatsReporter::TraceFormatter *Formatter);

  // In the general case we do not know how to format an entity for tracing.
  template <typename T>
  static const StatsReporter::TraceFormatter *getTraceFormatter() {
    return nullptr;
  }

public:
  StatsReporter *Reporter;
  llvm::TimeRecord SavedTime;
  llvm::StringRef EventName;
  const void *Entity;
  const StatsReporter::TraceFormatter *Formatter;
  FrontendStatsTracer();
  FrontendStatsTracer(FrontendStatsTracer &&other);
  FrontendStatsTracer &operator=(FrontendStatsTracer &&);
  ~FrontendStatsTracer();
  FrontendStatsTracer(const FrontendStatsTracer &) = delete;
  FrontendStatsTracer &operator=(const FrontendStatsTracer &) = delete;

  /// These are the convenience constructors you want to be calling throughout
  /// the compiler: they select an appropriate trace formatter for the provided
  /// entity type, and produce a tracer that's either active or inert depending
  /// on whether the provided \p Reporter is null (nullptr means "tracing is
  /// disabled").
  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName);

  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName,
                      const Decl *D);

  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName,
                      const clang::Decl *D);

  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName,
                      const Expr *E);

  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName,
                      const SourceFile *F);

  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName,
                      const Stmt *S);
  FrontendStatsTracer(StatsReporter *Reporter, llvm::StringRef EventName,
                      const Type *ty);
};

// In particular cases, we do know how to format traced entities: we declare
// explicit specializations of getTraceFormatter() here, matching the overloaded
// constructors of FrontendStatsTracer above, where the _definitions_ live in
// the upper-level files (in libswiftAST or libswiftSIL), and provide tracing
// for those entity types. If you want to trace those types, it's assumed you're
// linking with the object files that define the tracer.

template <>
const StatsReporter::TraceFormatter *
FrontendStatsTracer::getTraceFormatter<const Decl *>();

template <>
const StatsReporter::TraceFormatter *
FrontendStatsTracer::getTraceFormatter<const clang::Decl *>();

template <>
const StatsReporter::TraceFormatter *
FrontendStatsTracer::getTraceFormatter<const Expr *>();

template <>
const StatsReporter::TraceFormatter *
FrontendStatsTracer::getTraceFormatter<const SourceFile *>();

template <>
const StatsReporter::TraceFormatter *
FrontendStatsTracer::getTraceFormatter<const Stmt *>();

template <>
const StatsReporter::TraceFormatter *
FrontendStatsTracer::getTraceFormatter<const Type *>();

// Provide inline definitions for the delegating constructors.  These avoid
// introducing a circular dependency between libParse and libSIL.  They are
// marked as `inline` explicitly to prevent ODR violations due to multiple
// emissions.  We cannot force the inlining by defining them in the declaration
// due to the explicit template specializations of the `getTraceFormatter`,
// which is declared in the `FrontendStatsTracer` scope (the nested name
// specifier scope cannot be used to declare them).

inline FrontendStatsTracer::FrontendStatsTracer(StatsReporter *R,
                                                llvm::StringRef S)
    : FrontendStatsTracer(R, S, nullptr, nullptr) {}

inline FrontendStatsTracer::FrontendStatsTracer(StatsReporter *R,
                                                llvm::StringRef S,
                                                const Decl *D)
    : FrontendStatsTracer(R, S, D, getTraceFormatter<const Decl *>()) {}

inline FrontendStatsTracer::FrontendStatsTracer(StatsReporter *R,
                                                llvm::StringRef S,
                                                const clang::Decl *D)
    : FrontendStatsTracer(R, S, D, getTraceFormatter<const clang::Decl *>()) {}

inline FrontendStatsTracer::FrontendStatsTracer(StatsReporter *R,
                                                llvm::StringRef S,
                                                const Expr *E)
    : FrontendStatsTracer(R, S, E, getTraceFormatter<const Expr *>()) {}

inline FrontendStatsTracer::FrontendStatsTracer(StatsReporter *R,
                                                llvm::StringRef S,
                                                const SourceFile *SF)
    : FrontendStatsTracer(R, S, SF, getTraceFormatter<const SourceFile *>()) {}

inline FrontendStatsTracer::FrontendStatsTracer(StatsReporter *R,
                                                llvm::StringRef S,
                                                const Stmt *ST)
    : FrontendStatsTracer(R, S, ST, getTraceFormatter<const Stmt *>()) {}

inline FrontendStatsTracer::FrontendStatsTracer(StatsReporter *R,
                                                llvm::StringRef S,
                                                const Type *ty)
    : FrontendStatsTracer(R, S, ty, getTraceFormatter<const Type *>()) {}

/// Utilities for constructing TraceFormatters from entities in the
/// request-evaluator:

template <typename T>
typename std::enable_if<
    std::is_constructible<FrontendStatsTracer, StatsReporter *, llvm::StringRef,
                          const T *>::value,
    FrontendStatsTracer>::type
make_tracer_direct(StatsReporter *Reporter, llvm::StringRef Name, T *Value) {
  return FrontendStatsTracer(Reporter, Name, static_cast<const T *>(Value));
}

template <typename T>
typename std::enable_if<
    std::is_constructible<FrontendStatsTracer, StatsReporter *, llvm::StringRef,
                          const T *>::value,
    FrontendStatsTracer>::type
make_tracer_direct(StatsReporter *Reporter, llvm::StringRef Name,
                   const T *Value) {
  return FrontendStatsTracer(Reporter, Name, Value);
}

template <typename T>
typename std::enable_if<
    !std::is_constructible<FrontendStatsTracer, StatsReporter *,
                           llvm::StringRef, const T *>::value,
    FrontendStatsTracer>::type
make_tracer_direct(StatsReporter *Reporter, llvm::StringRef Name, T *Value) {
  return FrontendStatsTracer(Reporter, Name);
}

template <typename T>
typename std::enable_if<!std::is_pointer<T>::value, FrontendStatsTracer>::type
make_tracer_direct(StatsReporter *Reporter, llvm::StringRef Name, T Value) {
  return FrontendStatsTracer(Reporter, Name);
}

template <typename T> struct is_pointerunion : std::false_type {};
template <typename T, typename U>
struct is_pointerunion<llvm::PointerUnion<T, U>> : std::true_type {};

template <typename T, typename U>
FrontendStatsTracer make_tracer_pointerunion(StatsReporter *Reporter,
                                             llvm::StringRef Name,
                                             llvm::PointerUnion<T, U> Value) {
  if (Value.template is<T>())
    return make_tracer_direct(Reporter, Name, Value.template get<T>());
  else
    return make_tracer_direct(Reporter, Name, Value.template get<U>());
}

template <typename T>
typename std::enable_if<!is_pointerunion<T>::value, FrontendStatsTracer>::type
make_tracer_pointerunion(StatsReporter *Reporter, llvm::StringRef Name,
                         T Value) {
  return make_tracer_direct(Reporter, Name, Value);
}

template <typename First, typename... Rest>
FrontendStatsTracer make_tracer(StatsReporter *Reporter, llvm::StringRef Name,
                                std::tuple<First, Rest...> Value) {
  return make_tracer_pointerunion(Reporter, Name, std::get<0>(Value));
}

} // namespace stone
#endif