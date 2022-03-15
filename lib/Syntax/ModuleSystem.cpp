#include "stone/Syntax/ModuleSystem.h"

using namespace stone;
using namespace stone::syn;

ModuleSystem::ModuleSystem(Syntax &syntax) : syntax(syntax) {}

ModuleSystem::~ModuleSystem() {}

syn::Module *ModuleSystem::GetMainModule() {
  assert(false);
  return nullptr;
}

// bool ModuleSystem::CreateMainModule(llvm::StringRef name) {
//   auto identifier = syntax.GetSyntaxContext().GetIdentifier(name);
//   return true;
// }