#include "stone/Basic/PipelineEngine.h"

using namespace stone;

void PipelineEngine::Add(const PipelineListener *pipeline) {
  assert(pipeline && "Null pipeline");

  // entries.Add(
  //    std::make_pair<PipelineListenerKind, Pipeline>(pipeline->GetType(),
  //    pipeline));
}

PipelineListener *PipelineEngine::Get(PipelineListenerKind knd) {
  // for (auto p : entries) {
  //   if (p.first == kind) {
  //     return p.second;
  //   }
  // }
  return nullptr;
}
