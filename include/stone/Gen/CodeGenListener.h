#ifndef STONE_GEN_GENPIPELINE_H
#define STONE_GEN_GENPIPELINE_H

#include "stone/Core/PipelineListener.h"
#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
}
namespace stone {
namespace syn {
class Module;
}

class EmitIRListener : public PipelineListener {
public:
  EmitIRListener() : PipelineListener(PipelineListenerKind::EmittingIR) {}

public:
  virtual void OnEmitIRError() {}
  virtual void OnEmitIRCompleted(llvm::Module *m) {}

public:
};

class EmitObjectListener : public PipelineListener {
public:
  EmitObjectListener()
      : PipelineListener(PipelineListenerKind::EmittingObject) {}

public:
  virtual void OnEmitObjectError() {}
  virtual void OnEmitObjectCompleted() {}

public:
};

class EmitBitCodeListener : public PipelineListener {
public:
  EmitBitCodeListener()
      : PipelineListener(PipelineListenerKind::EmittingBitCode) {}

public:
  virtual void OnEmitBitCodeError() {}
  virtual void OnEmitBitCodeompleted() {}

public:
};

class EmitModuleListener : public PipelineListener {
public:
  EmitModuleListener()
      : PipelineListener(PipelineListenerKind::EmittingModule) {}

public:
  virtual void OnEmitModuleError() {}
  virtual void OnEmitModuleCompleted() {}

public:
};

class EmitLibraryListener : public PipelineListener {
public:
  EmitLibraryListener()
      : PipelineListener(PipelineListenerKind::EmittingLibrary) {}

public:
  virtual void OnEmitLibraryError() {}
  virtual void OnEmitLibraryCompleted() {}

public:
};

} // namespace stone
#endif
