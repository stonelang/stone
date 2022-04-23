#ifndef STONE_COMPILE_DEBUGLANGLISTENER_H
#define STONE_COMPILE_DEBUGLANGLISTENER_H

#include "stone/Compile/FrontendListener.h"

namespace stone {

class DebugFrontendListener final : public FrontendListener {
public:
  DebugFrontendListener();

  void OnCompileConfigured(Frontend &frontendInstance) override;
  void OnCompileStarted(Frontend &frontendInstance) override;
  void OnSyntaxAnalysisCompleted(Frontend &frontendInstance);
  void OnSemanticAnalysisCompleted(Frontend &frontendInstance);
  void OnCompileCompleted(Frontend &frontendInstance) override;
};

} // namespace stone

#endif
