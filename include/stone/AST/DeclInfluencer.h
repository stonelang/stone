#ifndef STONE_AST_DECLINFLUENCER_H
#define STONE_AST_DECLINFLUENCER_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Attribute.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/Visibility.h"
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

class alignas(1 << DeclAlignInBits) DeclInfluencer
    : public ASTAllocation<DeclInfluencer> {
  DeclInfluencerKind kind;
  SrcLoc loc;

public:
  DeclInfluencer(DeclInfluencerKind kind, SrcLoc loc) : kind(kind), loc(loc) {}

public:
  DeclInfluencerKind GetKind() { return kind; }
  SrcLoc GetLoc() { return loc; }

public:
  bool IsStatic() { return GetKind() == DeclInfluencerKind::Static; }
  bool IsExtern() { return GetKind() == DeclInfluencerKind::Extern; }
  bool IsVisibility() { return GetKind() == DeclInfluencerKind::Visibility; }
  bool IsAuto() { return GetKind() == DeclInfluencerKind::Auto; }
  bool IsInline() { return GetKind() == DeclInfluencerKind::Inline; }
  bool IsDeprecated() { return GetKind() == DeclInfluencerKind::Deprecated; }
};

class DeclModifier : public DeclInfluencer {
public:
  DeclModifier(DeclInfluencerKind kind, SrcLoc loc)
      : DeclInfluencer(kind, loc) {}
};

class StaticModifier : public DeclModifier {
public:
  StaticModifier(SrcLoc loc) : DeclModifier(DeclInfluencerKind::Static, loc) {}
};

class ExternModifier : public DeclModifier {
public:
  ExternModifier(SrcLoc loc) : DeclModifier(DeclInfluencerKind::Extern, loc) {}
};
class AutoModifier : public DeclModifier {
public:
  AutoModifier(SrcLoc loc) : DeclModifier(DeclInfluencerKind::Auto, loc) {}
};

class VisibilityModifier : public DeclModifier {
  VisibilityLevel visibilityLevel;

public:
  VisibilityModifier(VisibilityLevel level, SrcLoc loc)
      : DeclModifier(DeclInfluencerKind::Visibility, loc),
        visibilityLevel(level) {}

public:
  VisibilityLevel GetLevel() { return visibilityLevel; }
  void SetLevel(VisibilityLevel level) { visibilityLevel = level; }
};

class DeclAttribute : public DeclInfluencer, public Attribute {
public:
  DeclAttribute(DeclInfluencerKind kind, SrcLoc lBracketLoc, SrcRange range)
      : DeclInfluencer(kind, lBracketLoc), Attribute(lBracketLoc, range) {}
};

class InlineAttribute final : public DeclAttribute {
public:
  InlineAttribute(SrcLoc lBracketLoc, SrcRange range)
      : DeclAttribute(DeclInfluencerKind::Inline, lBracketLoc, range) {}
};

class DeprecatedAttribute final : public DeclAttribute {
public:
  DeprecatedAttribute(SrcLoc lBracketLoc, SrcRange range)
      : DeclAttribute(DeclInfluencerKind::Deprecated, lBracketLoc, range) {}
};

class AbstractDeclInfluencerList {
  //   llvm::BitVector mask;
  //   llvm::DenseMap<DeclInfluencerKind, DeclInfluencer *> influencers;

  // public:
  //   AbstractDeclInfluencerList()
  //       : mask(static_cast<unsigned>(DeclInfluencerKind::Last_Type) + 1) {}

  //   // Overload the bool operator
  //   explicit operator bool() const { return !influencers.empty(); }

  // public:
  //   void Add(DeclInfluencer *influencer) {
  //     influencers[influencer->GetKind()] = influencer;
  //     mask.set(static_cast<unsigned>(influencer->GetKind()));
  //   }
  //   bool Has(DeclInfluencerKind kind) const {
  //     return mask.test(static_cast<unsigned>(kind));
  //   }
  //   bool Has(DeclInfluencer *influencer) const {
  //     assert(influencer && "Cannot add null decl-influencer!");
  //     return Has(influencer->GetKind());
  //   }

  //   DeclInfluencer *Get(DeclInfluencerKind kind) const {
  //     auto it = influencers.find(kind);
  //     return it != influencers.end() ? it->second : nullptr;
  //   }
  //   bool IsEmpty() const { return influencers.size() == 0; }

public:
  void Add(DeclInfluencer *influencer) {}
  DeclInfluencer *Get(DeclInfluencerKind kind) const {}
  bool Has(DeclInfluencer *influencer) const {}
  bool Has(DeclInfluencerKind kind) const {}
};

class DeclInfluencerList final : public AbstractDeclInfluencerList {
  ASTContext &astContext;

public:
  explicit DeclInfluencerList(ASTContext &astContext)
      : astContext(astContext) {}

public:
  ASTContext &GetASTContext() { return astContext; }

public:
  void AddPublic(SrcLoc loc) {
    Add(new (astContext) VisibilityModifier(VisibilityLevel::Public, loc));
  }
  void AddPrivate(SrcLoc loc) {
    Add(new (astContext) VisibilityModifier(VisibilityLevel::Private, loc));
  }
  void AddInternal(SrcLoc loc) {
    Add(new (astContext) VisibilityModifier(VisibilityLevel::Internal, loc));
  }
  void AddStatic(SrcLoc loc) { Add(new (astContext) StaticModifier(loc)); }
  void AddExtern(SrcLoc loc) { Add(new (astContext) ExternModifier(loc)); }

  // void AddInline(SrcLoc loc) {
  //   GetDeclState()->Add(new (parser.GetASTContext())
  //                                       InlineAttribute(loc));
  // }
};

} // namespace stone

namespace llvm {

// template <> struct DenseMapInfo<stone::DeclInfluencerKind> {

//   static inline stone::DeclInfluencerKind getEmptyKey() {
//     return stone::DeclInfluencerKind::None;
//   }
//   static inline stone::DeclInfluencerKind getTombstoneKey() {
//     return static_cast<stone::DeclInfluencerKind>(
//         static_cast<uint8_t>(stone::DeclInfluencerKind::Last_Type) + 1);
//   }
//   static unsigned getHashValue(stone::DeclInfluencerKind kind) {
//     return static_cast<unsigned>(kind);
//   }
//   static bool isEqual(stone::DeclInfluencerKind lhs,
//                       stone::DeclInfluencerKind rhs) {
//     return lhs == rhs;
//   }
// };

} // namespace llvm
#endif
