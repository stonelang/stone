#ifndef STONE_SYNTAX_MODULESYSTEM_H
#define STONE_SYNTAX_MODULESYSTEM_H

#include "stone/Syntax/Module.h"

namespace stone {

namespace syn {
class ModuleSystem final {

  // We need built-in information
  TreeContext &tc;
  /// This is the main module that will be created
  mutable syn::Module *mainModule = nullptr;

public:
  ModuleSystem(TreeContext &tc);
  ~ModuleSystem();

  syn::Module *GetMainModule();

public:
  // void SetModuleName(llvm::StringRef name) {
  //    // langOpts.moduleName = name.str();
  //    // IRGenOpts.ModuleName = name.str();
  //  }

  // llvm::StringRef GetModuleName() const { return langOpts.moduleName; }
};
} // namespace syn
} // namespace stone
#endif
