#ifndef STONE_COMPILE_LANGLISTENER_H
#define STONE_COMPILE_LANGLISTENER_H

#include "stone/Basic/File.h"
#include "stone/Compile/SyntaxListener.h"
#include "stone/Compile/TypeCheckerListener.h"
#include "stone/Gen/CodeGenListener.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class LangInstance;
class LangInvocation;

class LangListener : public SyntaxListener,
                     public TypeCheckerListener,
                     public EmitIRListener,
                     public EmitObjectListener,
                     public EmitLibraryListener,
                     public EmitBitCodeListener,
                     public EmitModuleListener {
public:
  LangListener() {}
  virtual ~LangListener() = default;

public:
  virtual void OnCompileConfigured(LangInstance &lang) {}
  virtual void OnCompileStarted(LangInstance &lang) {}
  virtual void OnCompileCompleted(LangInstance &lang) {}
};

} // namespace stone
#endif
