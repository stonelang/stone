#ifndef STONE_CODEGEN_CODEGENPIPELINE_H
#define STONE_CODEGEN_CODEGENPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CodeGenPipeline : public Pipeline {
public:
  CodeGenPipeline() : Pipeline(PipelineType::CodeGen) {}

public:
};
} // namespace stone
#endif
