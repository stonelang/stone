#ifndef STONE_CODEGEN_CODEGENPIPELINE_H
#define STONE_CODEGEN_CODEGENPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CodeGenPipelineListener : public PipelineListener {
public:
  CodeGenPipelineListener() : PipelineListener(PipelineListenerKind::CodeGen) {}

public:
  virtual void OnIREmitted() {}
  virtual void OnObjectEmitted() {}
  virtual void OnBitCodeEmitted() {}
  virtual void OnLibraryEmitted() {}
  virtual void OnModuleEmitted() {}

public:
};

} // namespace stone
#endif
