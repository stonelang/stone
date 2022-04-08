#ifndef STONE_COMPILE_DEBUGLANGLISTENER_H
#define STONE_COMPILE_DEBUGLANGLISTENER_H

#include "stone/Compile/LangListener.h"

namespace stone {

class DebugLangListener final : public LangListener {
public:
  DebugLangListener();

  void OnCompileConfigured(LangInstance &lang) override;
  void OnCompileStarted(LangInstance &lang) override;
  void OnSyntaxAnalysisCompleted(LangInstance &lang);
  void OnSemanticAnalysisCompleted(LangInstance &lang);
  void OnCompileCompleted(LangInstance &lang) override;
};

} // namespace stone

#endif
