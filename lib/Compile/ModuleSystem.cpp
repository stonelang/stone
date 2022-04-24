#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/Frontend.h"

using namespace stone;
using namespace stone::syn;

ModuleSystem::ModuleSystem(Syntax &syntax, Context &ctx)
    : syntax(syntax), ctx(ctx) {}

ModuleSystem::~ModuleSystem() {}

syn::Module *ModuleSystem::GetMainModule() const {
  if (mainModule) {
    return mainModule;
  }
  Identifier &moduleName =
      syntax.GetSyntaxContext().GetIdentifier(GetModuleName());
  mainModule = syntax.MakeModuleDecl(moduleName, true);
  return mainModule;
}

void ModuleSystem::SetModuleName(llvm::StringRef name) {
  moduleOpts.moduleName = name.data();
}

const llvm::StringRef ModuleSystem::ModuleSystem::GetModuleName() const {
  return moduleOpts.moduleName;
}
// TODO: May want to move to Module
stone::Error ModuleSystem::IsValidModuleName(llvm::StringRef moduleName) {

  // TODO: 1. Lexer::IsIdentifier() -- keep in mind that this is special
  return stone::Error();
}