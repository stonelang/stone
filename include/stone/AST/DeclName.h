#ifndef STONE_AST_DECLNAME_H
#define STONE_AST_DECLNAME_H

#include "stone/AST/Identifier.h"
#include "stone/Basic/SrcLoc.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
class ASTContext;

enum class DeclNameKind : uint8_t {
  Identifier = 0,
  Constructor,
  Destructor,
  Operator,
  LiteralOperator,
  UsingDirective,
  Compound,
};

class DeclNameBase final {
  Identifier identifier;
  SrcLoc identifierLoc;

public:
  DeclNameBase() : DeclNameBase(Identifier()) {}
  DeclNameBase(Identifier I, SrcLoc identifierLoc = SrcLoc())
      : identifier(I), identifierLoc(identifierLoc) {}

public:
  bool IsValid() const { return !identifier.IsEmpty(); }
  const SrcLoc &GetLoc() const { return identifierLoc; }
  Identifier GetIdentifier() const { return identifier; }
  const void *GetAsOpaquePointer() const { return identifier.GetPointer(); }
  static DeclNameBase GetFromOpaquePointer(void *P) {
    return Identifier::GetFromOpaquePointer(P);
  }

public:
  bool operator==(const DeclNameBase &RHS) const {
    return identifier == RHS.identifier && identifierLoc == RHS.identifierLoc;
  }
  bool operator!=(const DeclNameBase &RHS) const { return !(*this == RHS); }
};

/// Represents a compound declaration name.
struct CompoundDeclName final
    : llvm::FoldingSetNode,
      private llvm::TrailingObjects<CompoundDeclName, Identifier> {
  friend TrailingObjects;
  friend class DeclName;

  size_t numArgs;
  DeclNameBase baseName;

  explicit CompoundDeclName(DeclNameBase baseName, size_t numArgs)
      : baseName(std::move(baseName)), numArgs(numArgs) {}

  llvm::ArrayRef<Identifier> GetArgumentNames() const {
    return {getTrailingObjects<Identifier>(), numArgs};
  }

  llvm::MutableArrayRef<Identifier> GetArgumentNames() {
    return {getTrailingObjects<Identifier>(), numArgs};
  }

  // /// Uniquing for the ASTContext.
  // static void Profile(llvm::FoldingSetNodeID &ID, DeclNameBase nameBase,
  //                     llvm::ArrayRef<Identifier> argumentNames);

  // void Profile(llvm::FoldingSetNodeID &id) {
  //   Profile(id, BaseName, GetArgumentNames());
  // }
};

class DeclName final {
  DeclNameKind kind;
  llvm::PointerUnion<DeclNameBase, CompoundDeclName *> nameBaseOrCompound;

public:
  DeclName()
      : kind(DeclNameKind::Identifier), nameBaseOrCompound(DeclNameBase()) {}

  explicit DeclName(DeclNameKind kind, DeclNameBase baseName)
      : kind(kind), nameBaseOrCompound(std::move(baseName)) {}

  explicit DeclName(CompoundDeclName *compound)
      : kind(DeclNameKind::Compound), nameBaseOrCompound(compound) {}

  DeclNameKind GetKind() const { return kind; }
  bool IsCompound() const { return kind == DeclNameKind::Compound; }
  bool IsSpecial() const {
    return kind == DeclNameKind::Constructor ||
           kind == DeclNameKind::Destructor || kind == DeclNameKind::Operator ||
           kind == DeclNameKind::LiteralOperator ||
           kind == DeclNameKind::UsingDirective;
  }

  const DeclNameBase &GetDeclNameBase() const {
    assert(!IsCompound() && "Cannot get DeclNameBase from a compound name");
    return nameBaseOrCompound.get<DeclNameBase>();
  }

  CompoundDeclName *GetCompoundName() const {
    assert(IsCompound() && "Cannot get compound name from a simple DeclName");
    return nameBaseOrCompound.get<CompoundDeclName *>();
  }

  static DeclName CreateConstructor(ASTContext &AC, SrcLoc loc);
  static DeclName CreateDestructor(ASTContext &AC, SrcLoc loc);
  static DeclName CreateOperator(ASTContext &AC, SrcLoc loc);
  static DeclName CreateCompound(ASTContext &AC, DeclNameBase base);
};

} // namespace stone
namespace llvm {

raw_ostream &operator<<(raw_ostream &OS, stone::DeclNameBase D);
raw_ostream &operator<<(raw_ostream &OS, stone::DeclName I);

// DeclBaseNames hash just like pointers.
template <> struct DenseMapInfo<stone::DeclNameBase> {
  static stone::DeclNameBase getEmptyKey() {
    return stone::Identifier::getEmptyKey();
  }
  static stone::DeclNameBase getTombstoneKey() {
    return stone::Identifier::getTombstoneKey();
  }
  static unsigned getHashValue(stone::DeclNameBase Val) {
    return DenseMapInfo<const void *>::getHashValue(Val.GetAsOpaquePointer());
  }
  static bool isEqual(stone::DeclNameBase LHS, stone::DeclNameBase RHS) {
    return LHS == RHS;
  }
};

// A DeclBaseName is "pointer like".
template <typename T> struct PointerLikeTypeTraits;
template <> struct PointerLikeTypeTraits<stone::DeclNameBase> {
public:
  static inline void *getAsVoidPointer(stone::DeclNameBase D) {
    return const_cast<void *>(D.GetAsOpaquePointer());
  }
  static inline stone::DeclNameBase getFromVoidPointer(void *P) {
    return stone::DeclNameBase::GetFromOpaquePointer(P);
  }
  enum {
    NumLowBitsAvailable =
        PointerLikeTypeTraits<stone::Identifier>::NumLowBitsAvailable
  };
};

} // namespace llvm

#endif
