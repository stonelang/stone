#ifndef STONE_COMPILE_LANGLISTENER_H
#define STONE_COMPILE_LANGLISTENER_H
#include "stone/Compile/SyntaxListener.h"
#include "stone/Compile/TypeCheckerListener.h"
#include "stone/Core/File.h"
#include "stone/Gen/CodeGenListener.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Lang;
class LangContext;
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
  virtual void OnCompileConfigured(Lang &lang) {}
  virtual void OnCompileStarted(Lang &lang) {}
  virtual void OnCompileCompleted(Lang &lang) {}
};

} // namespace stone
#endif
