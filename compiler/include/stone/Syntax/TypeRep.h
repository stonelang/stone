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

enum class TypeRepKind : UInt8 {
#define TYPEREP(ID, PARENT) ID,
#define LAST_TYPEREP(ID) Last_TypeRep = ID,
#include "TypeRepKind.def"
};
enum : unsigned {
  NumTypeReprKindBits =
      stone::CountBitsUsed(static_cast<unsigned>(TypeRepKind::Last_TypeRep))
};

/// Representation of a type as written in source.
class alignas(1 << TypeRepAlignInBits) TypeRep
    : public syn::SyntaxAllocation<TypeRep> {

  TypeRepKind kind;

  TypeRep(const TypeRep &) = delete;
  void operator=(const TypeRep &) = delete;

public:
  TypeRep() {}

public:
  /// Walk this type representation.
  TypeRep *Walk(SyntaxWalker &walker);
  TypeRep *Walk(SyntaxWalker &&walker) { return Walk(walker); }

public:
  SrcLoc GetLoc() const;
  SrcLoc GetStartLoc() const;
  SrcLoc GetEndLoc() const;
  SrcRange GetSrcRange() const;

public:
  void Print(raw_ostream &os,
             const PrintSyntaxOptions &printOpts = PrintSyntaxOptions()) const;
};

// You may not need this part.
class SpecifierTypeRep final : public TypeRep {

public:
  SpecifierTypeRep() {}

public:
  static SpecifierTypeRep *Create(SyntaxContext &sc);
};

/// Example: const int b = 0; TypeLoc will have QualifierTypeRep
class QualifierTypeRep : public TypeRep {
  // SrcLoc constLoc;
  // SrcLoc restricLoc;
  // SrcLoc volatileLoc;
  // SrcLoc pureLoc;
  // SrcLoc mutableLoc;
  // SrcLoc immutableLoc;
  TypeRep *type;

public:
  QualifierTypeRep() : type(nullptr) {}
  QualifierTypeRep(TypeRep *type) : type(type) {}

  // public:
  //   void AddConst(SrcLoc loc) { constLoc = loc; }
  //   void AddRestrict(SrcLoc loc) { restricLoc = loc; }
  //   void AddVolatile(SrcLoc loc) { volatileLoc = loc; }
  //   void AddPure(SrcLoc loc) { pureLoc = loc; }
  //   void AddMutable(SrcLoc loc) { mutableLoc = loc; }

  // public:
  //   bool IsConst() const { return constLoc.isValid(); }
  //   bool IsRestrict() const { return restricLoc.isValid(); }
  //   bool IsVolatile() const { return volatileLoc.isValid(); }
  //   bool IsPure() const { return pureLoc.isValid(); }

  //   SrcLoc GetConstLoc() const { return constLoc; }
  //   SrcLoc GetRestrictLoc() const { return restricLoc; }
  //   SrcLoc GetVolatileLoc() const { return volatileLoc; }
  //   SrcLoc GetPureLoc() const { return pureLoc; }
  //   SrcLoc GetMutableLoc() const { return mutableLoc; }

public:
  TypeRep *GetType() { return type; }

public:
  static QualifierTypeRep *Create(TypeRep *type, SyntaxContext &sc);
};

/// Particle particle where Particle is the type-identifier
class IdentifierTypeRep : public TypeRep {};

/// A parsed element within a tuple type.
struct TupleTypeRepElement final {
  Identifier name;
  SrcLoc nameLoc;
  Identifier secondName;
  SrcLoc secondNameLoc;
  SrcLoc underscoreLoc;
  SrcLoc colonLoc;
  TypeRep *type;
  SrcLoc trailingCommaLoc;

  TupleTypeRepElement() : type(nullptr) {}
  TupleTypeRepElement(TypeRep *type) : type(type) {}
};

class CompositionTypeRep final
    : public TypeRep,
      private llvm::TrailingObjects<CompositionTypeRep, TypeRep *> {
  friend TrailingObjects;

  SrcLoc firstTypeLoc;
  SrcRange compositionRange;

  CompositionTypeRep(llvm::ArrayRef<TypeRep *> types, SrcLoc firstTypeLoc,
                     SrcRange compositionRange)
      : /*TypeRep(TypeRepKind::Composition),*/ firstTypeLoc(firstTypeLoc),
        compositionRange(compositionRange) {

    // Bits.CompositionTypeRepr.NumTypes = Types.size();
    std::uninitialized_copy(types.begin(), types.end(),
                            getTrailingObjects<TypeRep *>());
  }

  // public:
  //   ArrayRef<TypeRepr *> getTypes() const {
  //     return {getTrailingObjects<TypeRepr*>(),
  //     Bits.CompositionTypeRepr.NumTypes};
  //   }
  //   SrcLoc getSrcLoc() const { return FirstTypeLoc; }
  //   SrcRange getCompositionRange() const { return CompositionRange; }

  //   static CompositionTypeRepr *create(const ASTContext &C,
  //                                      ArrayRef<TypeRepr*> Protocols,
  //                                      SrcLoc FirstTypeLoc,
  //                                      SrcRange CompositionRange);

  //   static CompositionTypeRepr *createEmptyComposition(ASTContext &C,
  //                                                      SrcLoc AnyLoc) {
  //     return CompositionTypeRepr::create(C, {}, AnyLoc, {AnyLoc, AnyLoc});
  //   }

  //   static bool classof(const TypeRepr *T) {
  //     return T->getKind() == TypeReprKind::Composition;
  //   }
  //   static bool classof(const CompositionTypeRepr *T) { return true; }

  // private:
  //   SrcLoc getStartLocImpl() const { return FirstTypeLoc; }
  //   SrcLoc getLocImpl() const { return CompositionRange.Start; }
  //   SrcLoc getEndLocImpl() const { return CompositionRange.End; }
  //   void printImpl(ASTPrinter &Printer, const PrintOptions &Opts) const;
  friend class TypeRep;
};

class ComponentIdentifierTypeRep : public IdentifierTypeRep {
  DeclNameLoc nameLoc;

  /// The declaration context from which the bound declaration was
  /// found. only valid if IdOrDecl is a TypeDecl.
  DeclContext *dc;

public:
  DeclNameLoc GetNameLoc() const { return nameLoc; }
  // TODO: DeclNameRef GetNameRef() const;

  //  void SetValue(TypeDecl *td, DeclContext *dc) {
  //   IdOrDecl = TD;
  //   this->DC = DC;
  // }
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

/// Why QualType
class TupleTypeRep final : public TypeRep {
public:
  static TupleTypeRep *Create();
};

// class TupleTypeRep final : public TypeRep,
//     private llvm::TrailingObjects<TupleTypeRep, TupleTypeRepElement,
//     Located<unsigned>> {

// };

/// May want to inherit from QualifierTypeRep

// Ex: public pure fun
class FunctionTypeRep : public TypeRep {
  QualifierTypeRep *resultTy;

public:
  FunctionTypeRep(QualifierTypeRep *resultTy) : resultTy(resultTy) {}

public:
  QualifierTypeRep *GetResultTypeRep() { return resultTy; }
};

/// All this may be covered by the IdentifierTypeRep
class BuiltinTypeRep : public TypeRep {};

class AbstractPointerTypeRep : public QualifierTypeRep {};

/// Wrapper for source info for pointers.
/// Ex: const int* p = null;
class PointerTypeRep : public AbstractPointerTypeRep {};

/// Wrapper for source info for block pointers.
class MemberPointerTypeRep : public AbstractPointerTypeRep {};

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
