#ifndef STONE_CODEGEN_CODEGENPIPELINE_H
#define STONE_CODEGEN_CODEGENPIPELINE_H

#include "stone/Basic/PipelineListener.h"
#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
}
namespace stone {
namespace syn {
class Module;
}

class EmittingListener : public PipelineListener {
public:
  EmittingListener() : PipelineListener(PipelineListenerKind::Emitting) {}

public:
  virtual void OnIREmitted(llvm::Module *m) {}
  virtual void OnObjectEmitted() {}
  virtual void OnBitCodeEmitted() {}
  virtual void OnLibraryEmitted() {}

public:
};

class EmittingIRListener : public PipelineListener {
public:
  EmittingIRListener() : PipelineListener(PipelineListenerKind::EmittingIR) {}

public:
  virtual void OnEmitIRCompleted(llvm::Module *m) {}

public:
};

class EmittingObjectListener : public PipelineListener {
public:
  EmittingObjectListener()
      : PipelineListener(PipelineListenerKind::EmittingObject) {}

public:
  virtual void OnEmitObjectCompleted() {}

public:
};

class EmittingBitCodeListener : public PipelineListener {
public:
  EmittingBitCodeListener()
      : PipelineListener(PipelineListenerKind::EmittingBitCode) {}

public:
  virtual void OnEmitBitCodeompleted() {}

public:
};

class EmittingModuleListener : public PipelineListener {
public:
  EmittingModuleListener()
      : PipelineListener(PipelineListenerKind::EmittingModule) {}

public:
  virtual void OnEmitModuleCompleted() {}

public:
};

class EmittingLibraryListener : public PipelineListener {
public:
  EmittingLibraryListener()
      : PipelineListener(PipelineListenerKind::EmittingLibrary) {}

public:
  virtual void OnEmitLibraryCompleted() {}

public:
};

} // namespace stone
#endif
