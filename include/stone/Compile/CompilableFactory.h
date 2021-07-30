#ifndef STONE_COMPILE_COMPILABLEFACTORY_H
#define STONE_COMPILE_COMPILABLEFACTORY_H

#include "stone/CodeGen/CodeGenPipelineListener.h"
#include "stone/Parse/SyntaxPipelineListener.h"
#include "stone/Semantics/TypeCheckerPipelineListener.h"

namespace stone {
class Compiler;
class Compilable;

struct CompilableFactory final {
  static std::unique_ptr<Compilable> MakeSyntaxParsing(Compiler &compiler);
  static std::unique_ptr<Compilable> MakeTypeChecking(Compiler &compiler);
  static std::unique_ptr<Compilable> MakeEmittingIR(Compiler &compiler);
  static std::unique_ptr<Compilable> MakeEmittingObject(Compiler &compiler);
  static std::unique_ptr<Compilable> MakeEmittingBitCode(Compiler &compiler);
  static std::unique_ptr<Compilable> MakeEmittingModule(Compiler &compiler);
  static std::unique_ptr<Compilable> MakeEmittingLibrary(Compiler &compiler);
  static std::unique_ptr<Compilable> MakeEmittingAssembly(Compiler &compiler);
};
} // namespace stone

#endif