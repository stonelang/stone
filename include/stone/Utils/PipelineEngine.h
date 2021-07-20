#ifndef STONE_UTILS_PIPELINEENGINE_H
#define STONE_UTILS_PIPELINEENGINE_H

#include <iostream>

#include "stone/Utils/Color.h"
#include "stone/Utils/List.h"
#include "stone/Utils/Mem.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"

namespace stone {

enum class PipelineType {
  Check,
  Parse,
  Lex,
  Gen,
};

class Pipeline {
  PipelineType ty;

public:
  Pipeline() = default;
  Pipeline(PipelineType ty) : ty(ty) {}
  virtual ~Pipeline();

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
