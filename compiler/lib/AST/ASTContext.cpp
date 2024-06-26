#include "stone/AST/ASTContext.h"
#include "stone/Support/DiagnosticEngine.h"
#include "stone/AST/Module.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"

#include <algorithm>
#include <memory>

using namespace stone;

ASTContext::ASTContext(LangOptions &langOpts, const SearchPathOptions &spOpts,
                       ClangContext &clangContext, DiagnosticEngine &de,
                       StatsReporter *stats)
    : MemoryContext(langOpts), langOpts(langOpts), searchPathOpts(spOpts),
      clangContext(clangContext), de(de), stats(stats), identifiers(allocator),
      builtin(*this) {}

ASTContext::~ASTContext() {
  for (auto &cleanup : cleanups) {
    cleanup();
  }
}

Identifier ASTContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.GetIdentifier(name);
}

void ASTContext::AddLoadedModule(ModuleDecl *mod) {
  assert(mod);
  // Add a loaded module using an actual module name (physical name
  // on disk), in case -module-alias is used (otherwise same).
  //
  // For example, if '-module-alias Foo=Bar' is passed in to the frontend,
  // and a source file has 'import Foo', a module called Bar (real name)
  // will be loaded and added to the map.
  loadedModules[mod->GetRealName()] = mod;
}

Identifier ASTContext::GetRealModuleName(Identifier key,
                                         ModuleAliasLookupOption option) const {

  auto found = moduleAliasMap.find(key);
  if (found == moduleAliasMap.end()) {
    return key; // No module aliasing was used, so just return the given key
  }
  // Found an entry
  auto value = found->second;

  // With the alwaysRealName option, look up the real name by treating
  // the given key as an alias; if the key's not an alias, return the key
  // itself since that's the real name.
  if (option == ModuleAliasLookupOption::AlwaysRealName) {
    return value.second ? value.first : key;
  }

  // With realNameFromAlias or aliasFromRealName option, only return the value
  // if the given key matches the description (whether it's an alias or real
  // name) by looking up the value.second (true if keyed by an alias). If not
  // matched, return an empty Identifier.
  if ((option == ModuleAliasLookupOption::RealNameFromAlias && !value.second) ||
      (option == ModuleAliasLookupOption::AliasFromRealName && value.second)) {
    return Identifier();
  }
  // Otherwise return the value found (whether the key is an alias or real name)
  return value.first;
}