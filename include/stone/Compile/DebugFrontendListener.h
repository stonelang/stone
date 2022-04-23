#ifndef STONE_COMPILE_DEBUGLANGLISTENER_H
#define STONE_COMPILE_DEBUGLANGLISTENER_H

#include "stone/Compile/FrontendListener.h"

namespace stone {

class DebugFrontendListener final : public FrontendListener {
public:
  DebugFrontendListener();

  void OnCompileConfigured(FrontendInstance &frontendInstance) override;
  void OnCompileStarted(FrontendInstance &frontendInstance) override;
  void OnSyntaxAnalysisCompleted(FrontendInstance &frontendInstance);
  void OnSemanticAnalysisCompleted(FrontendInstance &frontendInstance);
  void OnCompileCompleted(FrontendInstance &frontendInstance) override;
};

} // namespace stone

#endif
