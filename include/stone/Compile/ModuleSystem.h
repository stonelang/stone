#ifndef STONE_COMPILE_MODULESYSTEM_H
#define STONE_COMPILE_MODULESYSTEM_H

#include "stone/Compile/LangOptions.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

namespace stone {

class ModuleSystem final {
  // We need built-in information
  syn::Syntax &syntax;

  const LangOptions &langOpts;
  /// This is the main module that will be created
  mutable syn::Module *mainModule = nullptr;

public:
  ModuleSystem(syn::Syntax &syntax, const LangOptions &langOpts);

  ~ModuleSystem();

  syn::Module *GetMainModule() const;

public:
  // void SetModuleName(llvm::StringRef name) {
  //    // langOpts.moduleName = name.str();
  //    // IRGenOpts.ModuleName = name.str();
  //  }
  llvm::StringRef GetModuleName() const {
    return langOpts.systemOpts.moduleName;
  }
};

} // namespace stone
#endif
