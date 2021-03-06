#ifndef STONE_BASIC_STATISTICENGINE_H
#define STONE_BASIC_STATISTICENGINE_H

#include "stone/Basic/List.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"

#include <iostream>

// TODO:  Similar implementation to that of DiagnosticEngine
namespace stone {
class Basic;
class StatsPrinter {};
class StatsListener {};

class Stats {
  bool enabled = false;
  ConstList<Stats> deps;
  Basic &basic;

protected:
  const char *name = nullptr;

public:
  std::unique_ptr<llvm::Timer> timer;

public:
  Stats(const char *name, Basic &basic);
  virtual ~Stats() {}

public:
  const char *GetName() const { return name; }
  void Enable() { enabled = true; }
  void Disable() { enabled = false; }
  void AddDep(const Stats *stats) { deps.Add(stats); }
  ConstList<Stats> GetDeps() { return deps; }

  llvm::Timer &GetTimer() { return *timer.get(); }

  Basic &GetBasic() { return basic; }

public:
  virtual void Print() = 0;
};

// TODO: You can do something very similar to that of the DiagnosticEngine
class StatisticEngine final {
  llvm::SmallVector<Stats *, 4> entries;

public:
  StatisticEngine();
  ~StatisticEngine();

public:
  void Register(Stats *stats);
  /// Print all groups and entries in groups
  void Print();
};
} // namespace stone

#endif
