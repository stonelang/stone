#include "stone/Syntax/SyntaxContext.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Syntax/Module.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"

#include <algorithm>
#include <memory>

using namespace stone;
using namespace stone::syn;

SyntaxContext::SyntaxContext(stone::LangContext &lc,
                             const SearchPathOptions &spOpts,
                             ClangContext &clangContext)
    : lc(lc), searchPathOpts(spOpts), identifiers(allocator),
      builtinContext(*this), stats(new SyntaxContextStats(*this)),
      clangContext(clangContext) {

  lc.GetStatEngine().Register(stats.get());
}

SyntaxContext::~SyntaxContext() {
  for (auto &cleanup : cleanups) {
    cleanup();
  }
}

const BuiltinContext &SyntaxContext::GetBuiltinContext() const {
  return builtinContext;
}

void *syn::AllocateInSyntaxContext(size_t bytes, const SyntaxContext &ctx,
                                   mem::AllocationArena arena,
                                   unsigned alignment) {
  return ctx.Allocate(bytes, alignment /*, arena*/);
}

Identifier SyntaxContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.GetIdentifier(name);
}

void SyntaxContext::AddLoadedModule(ModuleDecl *mod) {
  assert(mod);
  // Add a loaded module using an actual module name (physical name
  // on disk), in case -module-alias is used (otherwise same).
  //
  // For example, if '-module-alias Foo=Bar' is passed in to the frontend,
  // and a source file has 'import Foo', a module called Bar (real name)
  // will be loaded and added to the map.
  loadedModules[mod->GetRealName()] = mod;
}

Identifier
SyntaxContext::GetRealModuleName(Identifier key,
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

size_t SyntaxContext::GetSizeOfMemUsed() const {
  return GetAllocator().getTotalMemory();
}

// llvm::BumpPtrAllocator &SyntaxContext::GetBumpAllocator() const {
//   return internal.allocator;
// }

void SyntaxContextStats::Print(ColorStream &stream) {}
