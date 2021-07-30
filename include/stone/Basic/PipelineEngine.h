#ifndef STONE_BASIC_PIPELINEENGINE_H
#define STONE_BASIC_PIPELINEENGINE_H

#include <iostream>

#include "stone/Basic/List.h"

namespace stone {

enum class PipelineType {
  Syntax,
  TypeCheck,
  CodeGen,
};

class Pipeline {
  PipelineType ty;

public:
  Pipeline() = default;
  Pipeline(PipelineType ty) : ty(ty) {}
  virtual ~Pipeline() {}

public:
  PipelineType GetType() { return ty; }
  virtual llvm::StringRef GetName() = 0;
};

class PipelineEngine final {
  ConstList<std::pair<PipelineType, Pipeline>> entries;

public:
  PipelineEngine();
  ~PipelineEngine();

public:
  void Add(const Pipeline *pipeline);
  // TODO: template<typename T> Get(PipelineType);
  Pipeline *Get(PipelineType ty);
};
} // namespace stone

#endif
