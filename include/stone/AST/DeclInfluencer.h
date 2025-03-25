#ifndef STONE_AST_DECL_INFLUENCER_H
#define STONE_AST_DECL_INFLUENCER_H

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

enum class DeclInfluencerKind : uint8_t {
  None = 0, // Default value for uninitialized or null properties
#define DECL_INFLUENCER(ID, Parent) ID,
#define LAST_DECL_INFLUENCER(ID) Last_Type = ID,
#define DECL_INFLUENCER_RANGE(ID, FirstID, LastID)                             \
  First_##ID##Type = FirstID, Last_##ID##Type = LastID,
#include "stone/AST/DeclInfluencerNode.def"
};

class alignas(1 << DeclInfluencerAlignInBits) DeclInfluencer
    : public ASTAllocation<Property> {
  DeclInfluencerKind kind;
  SrcLoc loc;

  // public:
  //   enum Flags : unsigned {
  //     None = 0,
  //     Public = 1 << 0,
  //     Private = 1 << 1,
  //     Internal = 1 << 2,
  //     Static = 1 << 3,
  //     Extern = 1 << 4,
  //     Inline = 1 << 5,
  //     Deprecated = 1 << 6,
  //   };

private:
  Flags fastFlags;

public:
  DeclInfluencer(DeclInfluencerKind kind, SrcLoc loc) : kind(kind), loc(loc) {}

public:
  DeclInfluencerKind GetKind() { return kind; }
  SrcLoc GetLoc() { return loc; }

  // protected:
  //   void AddFlag(Flags influencer) { flags |= fastFlags; }
  //   void RemoveFla(Flags influencer) { flags &= ~fastFlags; }

  // public:

  //   bool HasPublic() const { return fastFlags & Flags::Const; }
  //   bool HasPrivate() const { return fastFlags & Flags::Pure; }
  //   bool HasRestrict() const { return fastFlags & Flags::Restrict; }
  //   bool HasVolatile() const { return fastFlags & Flags::Volatile; }
  //   bool HasStone() const { return fastFlags & Flags::Stone; }

public:
};

#endif