#include "stone/Syntax/ModuleSystem.h"

using namespace stone;
using namespace stone::syn;

ModuleSystem::ModuleSystem(TreeContext &tc) : tc(tc) {}

ModuleSystem::~ModuleSystem() {}

syn::Module *ModuleSystem::GetMainModule() {
  assert(false);
  return nullptr;
}

// bool ModuleSystem::CreateMainModule(llvm::StringRef name) {
//   auto identifier = tc->GetIdentifier(name);
//   return true;
// }