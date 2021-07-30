#ifndef STONE_BASIC_PIPELINEENGINE_H
#define STONE_BASIC_PIPELINEENGINE_H

#include <iostream>

#include "stone/Basic/List.h"

namespace stone {

enum class PipelineListenerKind {
  Syntax,
  Lex,
  TypeCheck,
  CodeGen,
};

class PipelineListener {
  PipelineListenerKind kind;

public:
  PipelineListener() = default;
  PipelineListener(PipelineListenerKind kind) : kind(kind) {}
  virtual ~PipelineListener() {}

public:
  PipelineListenerKind GetKind() { return kind; }
};

class PipelineEngine final {
  ConstList<std::pair<PipelineListenerKind, PipelineListener>> entries;

public:
  PipelineEngine();
  ~PipelineEngine();

public:
  void Add(const PipelineListener *pipeline);
  // TODO: template<typename T> Get(PipelineListenerKind);
  PipelineListener *Get(PipelineListenerKind kind);
};

} // namespace stone

#endif
