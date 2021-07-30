#ifndef STONE_CODEGEN_CODEGENPIPELINE_H
#define STONE_CODEGEN_CODEGENPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
}
namespace stone {

class EmittingIRPipelineListener : public PipelineListener {
public:
  EmittingIRPipelineListener()
      : PipelineListener(PipelineListenerKind::EmittingIR) {}

public:
  virtual void OnIREmitted(llvm::Module *m) {}

public:
};

class EmittingObjectPipelineListener : public PipelineListener {
public:
  EmittingObjectPipelineListener()
      : PipelineListener(PipelineListenerKind::EmittingObject) {}

public:
  virtual void OnObjectEmitted() {}

public:
};

class EmittingBitCodePipelineListener : public PipelineListener {
public:
  EmittingBitCodePipelineListener()
      : PipelineListener(PipelineListenerKind::EmittingBitCode) {}

public:
  virtual void OnBitCodeEmitted() {}

public:
};

class EmittingModulePipelineListener : public PipelineListener {
public:
  EmittingModulePipelineListener()
      : PipelineListener(PipelineListenerKind::EmittingModule) {}

public:
  virtual void OnModuleEmitted() {}

public:
};

class EmittingLibraryPipelineListener : public PipelineListener {
public:
  EmittingLibraryPipelineListener()
      : PipelineListener(PipelineListenerKind::EmittingLibrary) {}

public:
  virtual void OnLibraryEmitted() {}

public:
};

} // namespace stone
#endif
