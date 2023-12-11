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

namespace stone {

// TODO: CompilerStatScope
class StatFormatter {
public:
  StatFormatter() {}
  virtual ~StatFormatter() {}

  virtual void TraceName(const void *entity, raw_ostream &OS) const = 0;
  virtual void TraceLoc(const void *entity, SrcMgr *srcMgr,
                        clang::SourceManager *clangSrcMgr,
                        raw_ostream &stream) const = 0;
};

class Stat {
public:
  UInt64 timeUSec;
  UInt64 liveUSec;
  bool isEntry;
  llvm::StringRef statName;
  llvm::StringRef counterName;
  UInt64 counterDelta;
  UInt64 counterValue;
  const void *entity;
  const StatFormatter *formatter;
};

class CompilerStat final : public Stat {
public:
  CompilerStat() {}
};

class DriverStat final : public Stat {
public:
  DriverStat() {}
};

class CompilerStatFormatter final : public StatFormatter {
public:
  void TraceName(const void *entity, raw_ostream &OS) const override;
  void TraceLoc(const void *entity, SrcMgr *srcMgr,
                clang::SourceManager *clangSrcMgr,
                raw_ostream &stream) const override;
};

class StatSystem;

enum class StatTracerKind {
  Driver,
  Compiler,
};

/*public MemoryAllocation<StatTracer>*/
class StatTracer {

protected:
  StatSystem &statSystem;
  llvm::StringRef statName;
  StatFormatter *formatter = nullptr;

public:
  StatTracer(StatSystem &statSystem, llvm::StringRef statName,
             StatFormatter *formatter)
      : statSystem(statSystem), statName(statName), formatter(formatter) {}
  ~StatTracer();
};

class CompilerStatTracer final : public StatTracer {

public:
  CompilerStatTracer(StatSystem &engine, llvm::StringRef statName,
                     StatFormatter *formatter = nullptr)
      : StatTracer(engine, statName, formatter) {}
  ~CompilerStatTracer();
};

class StatSystem {

public:
  struct DriverCounters final {
#define DRIVER_STAT(ID) int64_t ID;
#include "Stats.def"
#undef DRIVER_STAT
  };

  struct CompilerCounters final {
#define COMPILER_STAT(NAME, ID) int64_t ID;
#include "Stats.def"
#undef COMPILER_STAT
  };

public:
  StatSystem();

public:
  void SaveCompilerStat(const StatTracer &tracer, bool isEntry = false);

public:
  virtual void SaveStat(const StatTracer &tracer, bool isEntry = false) {}

public:
  DriverCounters &GetDriverCounters();
  CompilerCounters &GetCompilerCounters();

public:
  // void EnterStatTracer(StatTracerKind kind);
  // void ExitStatTracer(StatTracerKind kind);

  // StatTracer* CreateStatTracer(StatTracerKind kind);
};

class CompilerStatSystem final : public StatSystem {
  struct CompilerCounters final {
#define COMPILER_STAT(NAME, ID) int64_t ID;
#include "Stats.def"
#undef COMPILER_STAT
  };
public:
  CompilerCounters &GetCounters();
};

class DriverStatSystem final : public StatSystem {
public:
  struct DriverCounters final {
#define DRIVER_STAT(ID) int64_t ID;
#include "Stats.def"
#undef DRIVER_STAT
  };

  DriverCounters &GetCounters();
};

} // namespace stone

#endif
