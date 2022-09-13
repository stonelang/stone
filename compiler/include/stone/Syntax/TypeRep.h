#ifndef STONE_SYNTAX_TYPEREP_H
#define STONE_SYNTAX_TYPEREP_H

#include "stone/Syntax/SyntaxOptions.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
namespace syn {
class DeclContext;
class SyntaxWalker;

class TypeDecl;
class TupleTypeRep;
class IdentifierTypeRep;

/// Representation of a type as written in source.
class alignas(1 << TypeRepAlignInBits) TypeRep
    : public SyntaxAllocation<TypeRep> {

  TypeRep(const TypeRep &) = delete;
  void operator=(const TypeRep &) = delete;

public:
  /// Walk this type representation.
  TypeRep *Walk(SyntaxWalker &walker);
  TypeRep *Walk(SyntaxWalker &&walker) { return Walk(walker); }

public:
  void Print(raw_ostream &os,
             const PrintSyntaxOptions &printOpts = PrintSyntaxOptions()) const;
};

} // namespace syn
} // end namespace stone

namespace llvm {
static inline raw_ostream &operator<<(raw_ostream &os,
                                      stone::syn::TypeRep *typeRep) {
  typeRep->Print(os);
  return os;
}
} // end namespace llvm

#endif
