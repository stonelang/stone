#include "stone/Compile/CompilableFactory.h"
#include "stone/Compile/Compilable.h"
#include "stone/Compile/Compiler.h"

#include <memory>

using namespace stone;

std::unique_ptr<Compilable>
CompilableFactory::MakeSyntaxParsing(Compiler &compiler) {
  return std::make_unique<SyntaxParsing>(compiler);
}
std::unique_ptr<Compilable>
CompilableFactory::MakeTypeChecking(Compiler &compiler) {

  auto syntaxParsing = std::make_unique<SyntaxParsing>(compiler);
  auto typeChecking = std::make_unique<TypeChecking>(compiler);

  syntaxParsing->AddListener(std::move(typeChecking));
  return syntaxParsing;
}

std::unique_ptr<Compilable>
CompilableFactory::MakeEmittingIR(Compiler &compiler) {

  auto syntaxParsing = std::make_unique<SyntaxParsing>(compiler);
  auto typeChecking = std::make_unique<TypeChecking>(compiler);
  auto emittingIR = std::make_unique<EmittingIR>(compiler);

  typeChecking->AddListener(std::move(emittingIR));
  syntaxParsing->AddListener(std::move(typeChecking));

  return syntaxParsing;
}

std::unique_ptr<Compilable>
CompilableFactory::MakeEmittingObject(Compiler &compiler) {

  auto syntaxParsing = std::make_unique<SyntaxParsing>(compiler);
  auto typeChecking = std::make_unique<TypeChecking>(compiler);
  auto emittingIR = std::make_unique<EmittingIR>(compiler);
  auto emittingObject = std::make_unique<EmittingObject>(compiler);

  emittingIR->AddListener(std::move(emittingObject));
  typeChecking->AddListener(std::move(emittingIR));
  syntaxParsing->AddListener(std::move(typeChecking));

  return syntaxParsing;
}

std::unique_ptr<Compilable>
CompilableFactory::MakeEmittingBitCode(Compiler &compiler) {
  return std::make_unique<EmittingBitCode>(compiler);
}

std::unique_ptr<Compilable>
CompilableFactory::MakeEmittingModule(Compiler &compiler) {
  return std::make_unique<EmittingModule>(compiler);
}

std::unique_ptr<Compilable>
CompilableFactory::MakeEmittingAssembly(Compiler &compiler) {
  return std::make_unique<EmittingAssembly>(compiler);
}
