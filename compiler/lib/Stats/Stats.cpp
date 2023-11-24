#include "stone/Stats/Stats.h"

using namespace stone;

SystemStatisticEngine::SystemStatisticEngine() {}

StatisticTracer::~StatisticTracer() {}

void CompilerStatisticFormatter::TraceName(const void *entity,
                                           raw_ostream &OS) const {}

void CompilerStatisticFormatter::TraceLoc(const void *entity, SrcMgr *srcMgr,
                                          clang::SourceManager *clangSrcMgr,
                                          raw_ostream &stream) const {}

CompilerStatisticTracer::~CompilerStatisticTracer() {
  engine.SaveCompilerStatistic(*this);
}

void SystemStatisticEngine::SaveCompilerStatistic(const StatisticTracer &tracer,
                                                  bool isEntry) {}