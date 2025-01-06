#ifndef STONE_AST_ATTRIBUTE_H
#define STONE_AST_ATTRIBUTE_H

#include "stone/AST/AttributeKind.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {
class DeclAttributeList;

class alignas(1 << AttributeAlignInBits) Attribute
    : public MemoryAllocation<Attribute> {
public:
  /// The location of the '['.
  const SrcLoc lBracketLoc;

  /// The source range of the attribute.
  const SrcRange range;

  /// The location of the attribute.
  SrcLoc GetSrcLoc() const { return range.Start; }

  /// Return the source range of the attribute.
  SrcRange GetRange() const { return range; }

  SrcRange GetRangeWithLLBracket() const {
    if (lBracketLoc.isValid()) {
      return {lBracketLoc, range.End};
    }
    return range;
  }

  Attribute(const Attribute &) = delete;

protected:
  Attribute(SrcLoc lBracketLoc, SrcRange range)
      : lBracketLoc(lBracketLoc), range(range) {}
};

class DeclAttribute : public Attribute {
public:
};

class InlineDeclAttribute : public DeclAttribute {
public:
};
class ExternDeclAttribute : public DeclAttribute {
public:
};

/// Attributes that may be applied to declarations.
class DeclAttributeList {
  /// Linked list of declaration attributes.
  DeclAttribute *attributes;
};

class TypeAttribute : public Attribute {
public:
};

/// Attributes that may be applied to declarations.
class TypeAttributeList {
  /// Linked list of declaration attributes.
  TypeAttribute *attributes;
};

} // namespace stone

#endif