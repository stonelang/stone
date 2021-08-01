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

class EmittingIRListener : public PipelineListener {
public:
  EmittingIRListener() : PipelineListener(PipelineListenerKind::EmittingIR) {}

public:
  virtual void OnIREmitted(llvm::Module *m) {}

public:
};

class EmittingObjectListener : public PipelineListener {
public:
  EmittingObjectListener()
      : PipelineListener(PipelineListenerKind::EmittingObject) {}

public:
  virtual void OnObjectEmitted() {}

public:
};

class EmittingBitCodeListener : public PipelineListener {
public:
  EmittingBitCodeListener()
      : PipelineListener(PipelineListenerKind::EmittingBitCode) {}

public:
  virtual void OnBitCodeEmitted() {}

public:
};

class EmittingModuleListener : public PipelineListener {
public:
  EmittingModuleListener()
      : PipelineListener(PipelineListenerKind::EmittingModule) {}

public:
  virtual void OnModuleEmitted() {}

public:
};

class EmittingLibraryListener : public PipelineListener {
public:
  EmittingLibraryListener()
      : PipelineListener(PipelineListenerKind::EmittingLibrary) {}

public:
  virtual void OnLibraryEmitted() {}

public:
};

} // namespace stone
#endif
