#ifndef STONE_COMPILE_LANGLISTENER_H
#define STONE_COMPILE_LANGLISTENER_H

#include "stone/Basic/File.h"
#include "stone/Gen/CodeGenListener.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Sem/TypeCheckerListener.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Frontend;
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
  virtual void OnCompileConfigured(Frontend &frontend) {}
  virtual void OnCompileStarted(Frontend &frontend) {}
  virtual void OnSyntaxAnalysisCompleted(Frontend &frontend) {}
  virtual void OnSemanticAnalysisCompleted(Frontend &frontend) {}
  virtual void OnCompileCompleted(Frontend &frontend) {}
};

} // namespace stone
#endif
