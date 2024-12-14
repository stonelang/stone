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

class alignas(1 << AttributeAlignInBits) Attribute
    : public MemoryAllocation<Attribute> {
public:
  /// The location of the '[['.
  const SrcLoc llBracketLoc;

  /// The source range of the attribute.
  const SrcRange range;

  /// The location of the attribute.
  SrcLoc GetSrcLoc() const { return range.Start; }

  /// Return the source range of the attribute.
  SrcRange GetRange() const { return range; }

  SrcRange GetRangeWithLLBracket() const {
    if (llBracketLoc.isValid()) {
      return {llBracketLoc, range.End};
    }
    return range;
  }

  Attribute(const Attribute &) = delete;

protected:
  Attribute(SrcLoc llBracketLoc, SrcRange range)
      : llBracketLoc(llBracketLoc), range(range) {}
};

class DeclAttribute : public Attribute {};

class SpecializeAttribute : public DeclAttribute {};

class AttributeSpecs {
public:
  AttributeSpecs() {}
};

} // namespace stone

#endif