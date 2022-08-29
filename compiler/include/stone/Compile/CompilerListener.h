#ifndef STONE_COMPILE_LANGLISTENER_H
#define STONE_COMPILE_LANGLISTENER_H

#include "stone/Basic/File.h"
#include "stone/Gen/CodeGenListener.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Sem/TypeCheckerListener.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CompilerInvocation;
class CompilerInstance;

class CompilerListener : public SyntaxListener,
                         public TypeCheckerListener,
                         public EmitIRListener,
                         public EmitObjectListener,
                         public EmitLibraryListener,
                         public EmitBitCodeListener,
                         public EmitModuleListener {
public:
  CompilerListener() {}
  virtual ~CompilerListener() = default;

public:
  virtual void OnCompileConfigured(CompilerInvocation &invocation) {}
  virtual void OnCompileStarted(CompilerInstance &instance) {}
  virtual void OnSyntaxAnalysisCompleted(CompilerInstance &instance) {}
  virtual void OnSemanticAnalysisCompleted(CompilerInstance &instance) {}
  virtual void OnCompileCompleted(CompilerInstance &instance) {}
};

} // namespace stone
#endif
