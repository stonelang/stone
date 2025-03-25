#ifndef STONE_AST_TYPEINFLUENCER_H
#define STONE_AST_TYPEINFLUENCER_H

class alignas(1 << TypeInfluencerAlignInBits) TypeInfluencer
    : public ASTAllocation<TypeInfluencer> {
  TypeInfluencerKind kind;
  SrcLoc loc;

  // public:
  //   enum Flags : unsigned {
  //     None = 0,
  //     Const = 1 << 0,
  //     Pure = 1 << 1,
  //     Restrict = 1 << 2,
  //     Volatile = 1 << 3,
  //     Stone = 1 << 4,
  //   };

  // private:
  //   Flags flags;

public:
  TypeInfluencer(TypeInfluencerKind kind, SrcLoc loc) : kind(kind), loc(loc) {}

public:
  TypeInfluencerKind GetKind() { return kind; }
  SrcLoc GetLoc() { return loc; }

  // public:
  //   void AddFlag(Flags influencer) { flags |= influencer; }
  //   void RemoveFla(Flags influencer) { flags &= ~influencer; }
  //   bool HasConst() const { return flags & Flags::Const; }
  //   bool HasPure() const { return flags & Flags::Pure; }
  //   bool HasRestrict() const { return flags & Flags::Restrict; }
  //   bool HasVolatile() const { return flags & Flags::Volatile; }
  //   bool HasStone() const { return flags & Flags::Stone; }
};

#endif