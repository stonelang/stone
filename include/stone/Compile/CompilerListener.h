#ifndef STONE_COMPILE_COMPILERLISTENER_H
#define STONE_COMPILE_COMPILERLISTENER_H

#include "stone/CodeGen/CodeGenListener.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Semantics/TypeCheckerListener.h"

namespace stone {
class Compiler;
class CompilerListener : public SyntaxListener,
                         public TypeCheckerListener,
                         public EmittingIRListener,
                         public EmittingObjectListener,
                         public EmittingLibraryListener,
                         public EmittingBitCodeListener,
                         public EmittingModuleListener {
public:
  CompilerListener() {}
  virtual ~CompilerListener() = default;

public:
  virtual void OnPreCompile(Compiler &compiler) {}
  virtual void OnPostCompile(Compiler &compiler) {}
};

} // namespace stone
#endif
