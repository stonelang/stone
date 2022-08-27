#ifndef STONE_COMPILE_DEBUGLANGLISTENER_H
#define STONE_COMPILE_DEBUGLANGLISTENER_H

#include "stone/Compile/CompilerListener.h"

namespace stone {

class DebugCompilerListener final : public CompilerListener {
public:
  DebugCompilerListener();

  void OnCompileConfigured(CompilerInvocation &invocation) override;
  void OnCompileStarted(CompilerInstance &instance) override;
  void OnSyntaxAnalysisCompleted(CompilerInstance &instance) override;
  void OnSemanticAnalysisCompleted(CompilerInstance &instance) override;
  void OnCompileCompleted(CompilerInstance &instance) override;
};

} // namespace stone

#endif
