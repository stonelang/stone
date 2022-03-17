#ifndef STONE_COMPILE_DEBUGLANGLISTENER_H
#define STONE_COMPILE_DEBUGLANGLISTENER_H

#include "stone/Compile/LangListener.h"

namespace stone {

class DebugLangListener final : public LangListener {

public:
  DebugLangListener();

  void OnCompileConfigured(Lang &lang) override;
  void OnCompileStarted(Lang &lang) override;
  void OnCompileCompleted(Lang &lang) override;
};

} // namespace stone

#endif
