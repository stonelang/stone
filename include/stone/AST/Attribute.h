#ifndef STONE_AST_ATTRIBUTE_H
#define STONE_AST_ATTRIBUTE_H

#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {

class Attribute {
  /// The location of the '['.
  SrcLoc lBracketLoc;

  /// The source range of the attribute.
  SrcRange range;

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

} // namespace stone

#endif