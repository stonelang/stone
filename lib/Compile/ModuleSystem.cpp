#include "stone/Compile/ModuleSystem.h"

using namespace stone;
using namespace stone::syn;

ModuleSystem::ModuleSystem(Syntax &syntax, const LangOptions &langOpts)
    : syntax(syntax), langOpts(langOpts) {}

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