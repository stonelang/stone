#ifndef STONE_SYNTAX_TYPEREP_H
#define STONE_SYNTAX_TYPEREP_H

#include "stone/Syntax/DeclName.h"
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

class DeclNameLoc;
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
  TypeRep() {}

public:
  /// Walk this type representation.
  TypeRep *Walk(SyntaxWalker &walker);
  TypeRep *Walk(SyntaxWalker &&walker) { return Walk(walker); }

public:
  void Print(raw_ostream &os,
             const PrintSyntaxOptions &printOpts = PrintSyntaxOptions()) const;
};

/// Example: const int b = 0; TypeLoc will have QualTypeRep
class QualTypeRep : public TypeRep {
  SrcLoc constLoc;
  SrcLoc restricLoc;
  SrcLoc volatileLoc;
  SrcLoc pureLoc;

public:
  bool IsConst() const { return constLoc.isValid(); }
  bool IsRestrict() const { return restricLoc.isValid(); }
  bool IsVolatile() const { return volatileLoc.isValid(); }
  bool IsPure() const { return pureLoc.isValid(); }

  SrcLoc GetConstLoc() const { return constLoc; }
  SrcLoc GetRestrictLoc() const { return restricLoc; }
  SrcLoc GetVolatileLoc() const { return volatileLoc; }
  SrcLoc GetPureLoc() const { return pureLoc; }
};

class IdentifierTypeRep : public QualTypeRep {};

class ComponentIdentifierTypeRep : public IdentifierTypeRep {
  DeclNameLoc nameLoc;

public:
  DeclNameLoc GetNameLoc() const { return nameLoc; }
  // TODO: DeclNameRef GetNameRef() const;
};

/// A simple identifier type like "int".
class BasicIdenifiertTypeRep : public ComponentIdentifierTypeRep {
public:
  // BasicIdenifiertTypeRep(DeclNameLoc Loc, DeclNameRef Id)
  //   : ComponentIdentTypeRepr(TypeReprKind::SimpleIdent, Loc, Id) {}

  // // SmallVector::emplace_back will never need to call this because
  // // we reserve the right size, but it does try statically.
  // BasicIdenifiertTypeRep(const SimpleIdentTypeRepr &repr)
  //     : SimpleIdentTypeRepr(repr.getNameLoc(), repr.getNameRef()) {
  //   llvm_unreachable("should not be called dynamically");
  // }

  // static bool classof(const TypeRepr *T) {
  //   return T->getKind() == TypeReprKind::SimpleIdent;
  // }
  // static bool classof(const SimpleIdentTypeRepr *T) { return true; }

private:
  /// TODO:
  // SrcLoc GetStartLocImpl() const { return GetNameLoc().GetStartLoc(); }
  // SrcLoc GetEndLocImpl() const { return GetNameLoc().GetEndLoc(); }

  friend class TypeRep;
};

class TupleTypeRep final : public QualTypeRep {

public:
public:
  static TupleTypeRep *Create();
};

/// May want to inherit from QualTypeRep

// Ex: public pure fun
class FunctionTypeRep : public TypeRep {
  QualTypeRep *resultTy;

public:
  FunctionTypeRep(QualTypeRep *resultTy) : resultTy(resultTy) {}

public:
  QualTypeRep *GetResultTypeRep() { return resultTy; }
};

/// All this may be covered by the IdentifierTypeRep
class BuiltinTypeRep : public TypeRep {};

class AbstractPointerTypeRep : public QualTypeRep {};

/// Wrapper for source info for pointers.
/// Ex: const int* p = null;
class PointerTypeRep : public AbstractPointerTypeRep {};

/// Wrapper for source info for block pointers.
class BlockPointerTypeRep : public AbstractPointerTypeRep {};

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
