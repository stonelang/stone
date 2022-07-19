#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/Frontend.h"
#include "stone/Parse/Lexer.h"

using namespace stone;
using namespace stone::syn;

ModuleSystem::ModuleSystem(Syntax &syntax, Context &ctx,
                           ModuleOptions &moduleOpts)
    : syntax(syntax), ctx(ctx), moduleOpts(moduleOpts) {}

ModuleSystem::~ModuleSystem() {}

syn::Module *ModuleSystem::GetMainModule() const {
  if (mainModule) {
    return mainModule;
  }
  Identifier &moduleIdentifier =
      syntax.GetSyntaxContext().GetIdentifier(moduleOpts.moduleName);
  mainModule = syntax.MakeModuleDecl(moduleIdentifier, true);
  return mainModule;
}

stone::Error ModuleSystem::IsValidModuleName(const llvm::StringRef moduleName) {
  if (!Lexer::isIdentifier(moduleName)) {
    return stone::Error(true);
  }
  return stone::Error();
}