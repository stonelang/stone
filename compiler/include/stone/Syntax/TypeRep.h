#ifndef STONE_SYNTAX_TYPEREP_H
#define STONE_SYNTAX_TYPEREP_H

#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/DeclContext.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
namespace syn {
class QualType;

// class Qualifier;

enum class TypeRepKind : uint8_t {
#define TYPEREP(ID, PARENT) ID,
#define LAST_TYPEREP(ID) LastTypeRep = ID,
#include "TypeRepKind.def"
};

class alignas(1 << TypeRepAlignInBits) TypeRep
    : public SyntaxAllocation<TypeRep> {

  TypeRep(const TypeRep &) = delete;
  void operator=(const TypeRep &) = delete;

public:
};

class QualTypeRep : public TypeRep {
public:
};

class FunctionTypeRep : public TypeRep {
public:
};

} // namespace syn
} // namespace stone

#endif