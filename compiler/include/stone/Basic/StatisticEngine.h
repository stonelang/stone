#ifndef STONE_BASIC_STATISTICENGINE_H
#define STONE_BASIC_STATISTICENGINE_H

#include <iostream>

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Timer.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"

// TODO:  Similar implementation to that of DiagnosticEngine
namespace stone {

class StatsPrinter {};
class StatsListener {};

class Stats {
  bool enabled = false;
  ConstList<Stats> deps;

protected:
  const char *name = nullptr;

public:
  std::unique_ptr<stone::Timer> timer;

public:
  Stats(const char *name);
  virtual ~Stats() {}

public:
  const char *GetName() const { return name; }
  void Enable() { enabled = true; }
  void Disable() { enabled = false; }
  void AddDep(const Stats *stats) { deps.Add(stats); }
  ConstList<Stats> GetDeps() { return deps; }
  stone::Timer &GetTimer() { return *timer.get(); }

public:
  virtual void Print(ColorStream &stream) = 0;
};

// class StatisticFormatter {
// public:
// };

// class Statistic {};

// class StatisticTracer {
// public:
//   StatisticTracer(StatisticEngine *engine, llvm::StringRef eventName,
//                   StatisticFormatter *formatter);
//   ~StatisticTracer();
// };

// TODO: You can do something very similar to that of the DiagnosticEngine
class StatisticEngine final {
  llvm::SmallVector<Stats *, 4> entries;

public:
  StatisticEngine();
  ~StatisticEngine();

public:
  void Register(Stats *stats);
  /// Print all groups and entries in groups
  void Print(ColorStream &stream);
};
} // namespace stone

#endif
