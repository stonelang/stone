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

class StatisticFormatter {
public:
  StatisticFormatter() {}
  virtual ~StatisticFormatter() {}

  virtual void TraceName(const void *entity, raw_ostream &OS) const = 0;
  virtual void TraceLoc(const void *entity, SrcMgr *srcMgr,
                        clang::SourceManager *clangSrcMgr,
                        raw_ostream &stream) const = 0;
};

class Statistic {
public:
};

class CompilerStatistic : public Statistic {
public:
  UInt64 timeUSec;
  UInt64 liveUSec;
  bool isEntry;
  llvm::StringRef statName;
  llvm::StringRef counterName;
  UInt64 counterDelta;
  UInt64 counterValue;
  const void *entity;
  const StatisticFormatter *formatter;
};

class CompilerStatisticFormatter final : public StatisticFormatter {
public:
  void TraceName(const void *entity, raw_ostream &OS) const override;
  void TraceLoc(const void *entity, SrcMgr *srcMgr,
                clang::SourceManager *clangSrcMgr,
                raw_ostream &stream) const override;
};

class SystemStatisticEngine;

class StatisticTracer {

protected:
  SystemStatisticEngine &engine;
  llvm::StringRef statName;
  StatisticFormatter *formatter = nullptr;

public:
  StatisticTracer(SystemStatisticEngine &engine, llvm::StringRef statName,
                  StatisticFormatter *formatter)
      : engine(engine), statName(statName), formatter(formatter) {}
  ~StatisticTracer();
};

class CompilerStatisticTracer final : public StatisticTracer {

public:
  CompilerStatisticTracer(SystemStatisticEngine &engine,
                          llvm::StringRef statName,
                          StatisticFormatter *formatter = nullptr)
      : StatisticTracer(engine, statName, formatter) {}
  ~CompilerStatisticTracer();
};

class SystemStatisticEngine final {

public:
  struct DriverCounters final {
#define DRIVER_STATISTIC(ID) int64_t ID;
#include "Stats.def"
#undef DRIVER_STATISTIC
  };

  struct CompilerCounters final {
#define COMPILER_STATISTIC(NAME, ID) int64_t ID;
#include "Stats.def"
#undef COMPILER_STATISTIC
  };

public:
  SystemStatisticEngine();

public:
  void SaveCompilerStatistic(const StatisticTracer &tracer,
                             bool isEntry = false);

public:
  DriverCounters &GetDriverCounters();
  CompilerCounters &GetCompilerCounters();
};

} // namespace stone

#endif
