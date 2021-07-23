#include "stone/Basic/StatisticEngine.h"

using namespace stone;

Stats::Stats(const char *name, Basic &basic) : name(name), basic(basic) {
  timer.reset(new llvm::Timer(name, name));
}

StatisticEngine::StatisticEngine() {}
StatisticEngine::~StatisticEngine() {}

void StatisticEngine::Register(Stats *stats) {
  assert(stats && "Unable to register null stats.");
  entries.push_back(stats);
}

void StatisticEngine::Print() {
  for (const auto &stats : entries) {
    stats->Print();
  }
}
