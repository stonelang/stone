#ifndef STONE_AST_PROPERTY_H
#define STONE_AST_PROPERTY_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/AttributeKind.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

#include "stone/Basic/SrcLoc.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <cstdint>
#include <vector>

namespace stone {

enum class PropertyKind : uint8_t {
  None = 0, // Default value for uninitialized or null properties
#define PROPERTY(id, parent) id,
#define LAST_PROPERTY(id) Last_Type = id,
#define PROPERTY_RANGE(Id, FirstId, LastId)                                    \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/AST/PropertyNode.def"
};

}
#endif

