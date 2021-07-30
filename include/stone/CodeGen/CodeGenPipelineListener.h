#ifndef STONE_CODEGEN_CODEGENPIPELINE_H
#define STONE_CODEGEN_CODEGENPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {

class EmittingIRPipelineListener : public PipelineListener {
public:
  EmittingIRPipelineListener()
      : PipelineListener(PipelineListenerKind::CodeGen) {}

public:
  virtual void OnIREmitted() {}

public:
};

class EmittingObjectPipelineListener : public PipelineListener {
public:
  EmittingObjectPipelineListener()
      : PipelineListener(PipelineListenerKind::CodeGen) {}

public:
  virtual void OnObjectEmitted() {}

public:
};

class EmittingBitCodePipelineListener : public PipelineListener {
public:
  EmittingBitCodePipelineListener()
      : PipelineListener(PipelineListenerKind::CodeGen) {}

public:
  virtual void OnBitCodeEmitted() {}

public:
};

class EmittingModulePipelineListener : public PipelineListener {
public:
  EmittingModulePipelineListener()
      : PipelineListener(PipelineListenerKind::CodeGen) {}

public:
  virtual void OnModuleEmitted() {}

public:
};

class EmittingLibraryPipelineListener : public PipelineListener {
public:
  EmittingLibraryPipelineListener()
      : PipelineListener(PipelineListenerKind::CodeGen) {}

public:
  virtual void OnLibraryEmitted() {}

public:
};

} // namespace stone
#endif
