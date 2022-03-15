#ifndef STONE_CORE_PIPELINEENGINE_H
#define STONE_CORE_PIPELINEENGINE_H

#include <iostream>

#include "stone/Core/List.h"

namespace stone {

enum class PipelineListenerKind {
  Lexing,
  Parsing,
  TypeChecking,
  EmittingIR,
  EmittingObject,
  EmittingModule,
  EmittingBitCode,
  EmittingLibrary
};

class PipelineListener {
  PipelineListenerKind kind;

public:
  PipelineListener() = default;
  PipelineListener(PipelineListenerKind kind) : kind(kind) {}
  virtual ~PipelineListener() = default;

public:
  PipelineListenerKind GetKind() { return kind; }
};

} // namespace stone

#endif
