#ifndef STONE_STATS_STATIS_H
#define STONE_STATS_STATIS_H

#include <iostream>

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Timer.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"

// TODO:  Similar implementation to that of DiagnosticEngine
namespace stone {

class StatisticFormatter {
public:
};

class Statistic {
public:

};
// class StatisticTracer {
// public:
//   StatisticTracer(StatisticEngine *engine, llvm::StringRef eventName,
//                   StatisticFormatter *formatter);
//   ~StatisticTracer();
// };

// class CompilerStatisticTracer : public StatisticTracer {

// };

// class DriverStatisticTracer : public StatisticTracer {

// };
// TODO: You can do something very similar to that of the DiagnosticEngine
// class StatisticEngine final {
  
// public:
//   StatisticEngine();
//   ~StatisticEngine();
// };
} // namespace stone

#endif
