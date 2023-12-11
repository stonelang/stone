#include "stone/Stats/Stats.h"

using namespace stone;

StatSystem::StatSystem() {}

StatTracer::~StatTracer() {}

void StatSystem::SaveCompilerStat(const StatTracer &tracer,
                                                  bool isEntry) {}


void CompilerStatFormatter::TraceName(const void *entity,
                                           raw_ostream &OS) const {}

void CompilerStatFormatter::TraceLoc(const void *entity, SrcMgr *srcMgr,
                                          clang::SourceManager *clangSrcMgr,
                                          raw_ostream &stream) const {}

CompilerStatTracer::~CompilerStatTracer() {
  //statSystem.SaveStat(*this);
}

