#include "stone/Syntax/Decl.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/Template.h" //DeclTemplate
#include "stone/Syntax/Type.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

using namespace stone;
using namespace stone::syn;

// Only allow allocation of Decls using the allocator in ASTContext.
void *syn::Decl::operator new(std::size_t bytes, const TreeContext &tc,
                              unsigned alignment) {
  return tc.Allocate(bytes, alignment);
}

// Only allow allocation of Modules using the allocator in ASTContext.
void *syn::Module::operator new(std::size_t bytes, const TreeContext &tc,
                                unsigned alignment) {
  return tc.Allocate(bytes, alignment);
}

FunctionDecl::FunctionDecl(DeclKind ty, SrcLoc loc, TreeContext &tc,
                           DeclContext *dc)
    : DeclaratorDecl(ty, loc, dc), DeclContext(DeclContextKind::Decl, ty) {}

void DeclStats::Print() {}
