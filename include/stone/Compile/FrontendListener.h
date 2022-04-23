#ifndef STONE_COMPILE_LANGLISTENER_H
#define STONE_COMPILE_LANGLISTENER_H

#include "stone/Basic/File.h"
#include "stone/Gen/CodeGenListener.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Sem/TypeCheckerListener.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class FrontendInstance;
class FrontendInvocation;

class FrontendListener : public SyntaxListener,
                         public TypeCheckerListener,
                         public EmitIRListener,
                         public EmitObjectListener,
                         public EmitLibraryListener,
                         public EmitBitCodeListener,
                         public EmitModuleListener {
public:
  FrontendListener() {}
  virtual ~FrontendListener() = default;

public:
  virtual void OnCompileConfigured(FrontendInstance &lang) {}
  virtual void OnCompileStarted(FrontendInstance &lang) {}
  virtual void OnSyntaxAnalysisCompleted(FrontendInstance &lang) {}
  virtual void OnSemanticAnalysisCompleted(FrontendInstance &lang) {}
  virtual void OnCompileCompleted(FrontendInstance &lang) {}
};

} // namespace stone
#endif
