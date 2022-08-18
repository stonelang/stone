#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

#include "stone/Foreign/Foreign.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"
#include "stone/Syntax/TypeLoc.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/type_traits.h"


#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

#include <string>

namespace stone {
namespace syn {

enum class GCKind : uint8_t { None = 0, Weak, Strong };

// Extended Qualifiers
class ExtQuals;
// Qualified Types

class QualType;
class StructDecl;

class Type;

class TypeContext final {
public:
};

class alignas(1 << TypeAlignInBits) Type
    : public SyntaxAllocation<std::aligned_storage<8, 8>::type> {};

// TODO: QualType to TypeRep
class alignas(1 << QualTypeAlignInBits) QualType
    : public SyntaxAllocation<QualType> {
public:
  QualType() = default;
  QualType(const Type *ty, unsigned quals) {}

public:
  /// Retrieves a pointer to the underlying (unqualified) type.
  ///
  /// This function requires that the type not be NULL. If the type might be
  /// NULL, use the (slightly less efficient) \c getTypePtrOrNull().
  const Type *GetTypePtr() const;

  const Type *GetTypePtrOrNull() const;
};

class FunctionTypeBase : public Type {};

class FunctionType : public FunctionTypeBase {
  // The type returned by the function.
  QualType returnType;

public:
  QualType GetReturnType() { return returnType; }
};

class NominalType : public Type {};

class StructType : public NominalType {};

class EnumType : public NominalType {};

class DeducedType : public Type {};

class alignas(8) AutoType : public DeducedType, public llvm::FoldingSetNode {
  friend class SyntaxContext; // SyntaxContext creates these
};

class BuiltinType : public Type {};

using TypeRep = OpaquePtr<QualType>;

using UnionTypeRep = UnionOpaquePtr<QualType>;

} // namespace syn
} // namespace stone
#endif
