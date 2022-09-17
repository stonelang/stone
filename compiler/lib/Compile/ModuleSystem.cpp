#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Parse/Lexer.h"

#include "stone/Syntax/SyntaxFactory.h"

using namespace stone;
using namespace stone::syn;

ModuleSystem::ModuleSystem(LangContext &lc, SyntaxContext &sc,
                           ModuleOptions &moduleOpts)
    : lc(lc), sc(sc), moduleOpts(moduleOpts) {}

ModuleSystem::~ModuleSystem() {}

syn::Module *ModuleSystem::GetMainModule() const {
  if (mainModule) {
    return mainModule;
  }
  Identifier &moduleIdentifier = sc.GetIdentifier(moduleOpts.moduleName);
  mainModule = ModuleDeclFactory::Create(&moduleIdentifier, sc, true);
  return mainModule;
}

stone::Error ModuleSystem::IsValidModuleName(const llvm::StringRef moduleName) {

  llvm::SmallVector<llvm::StringRef, 4> results;
  moduleName.split(results, ".");
  for (auto identifier : results) {
    if (!Lexer::isIdentifier(identifier)) {
      return stone::Error(true);
    }
  }
  return stone::Error();
}