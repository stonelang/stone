#include "stone/Core/StatisticEngine.h"

using namespace stone;

Stats::Stats(const char *name) : name(name) {
  timer = std::make_unique<stone::Timer>(name, name);
}

StatisticEngine::StatisticEngine() {}
StatisticEngine::~StatisticEngine() {}

void StatisticEngine::Register(Stats *stats) {
  assert(stats && "Unable to register null stats.");
  entries.push_back(stats);
}

void StatisticEngine::Print(ColorfulStream &stream) {
  for (const auto &stats : entries) {
    stats->Print(stream);
  }
}
