#ifndef STONE_AST_QUALSPECS_H
#define STONE_AST_QUALSPECS_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"

namespace stone {
class Type;
class DeclSpec;
class QualType;
class ASTContext;

class QualSpecs final {
  friend QualType;
  friend DeclSpec;
  friend ASTContext;

  unsigned qualSpecs;
  QualSpecs() : qualSpecs(0) {}

public:
  enum Flags : unsigned {
    None = 1 << 0,
    Const = 1 << 1,
    Restrict = 1 << 2,
    Volatile = 1 << 3,
    Stone = 1 << 4,
    Mutable = 1 << 5
  };

public:
  ///\Has const qualifier
  bool HasConst() const { return qualSpecs & Flags::Const; }
  ///\Has only const qualifier
  bool IsConst() const { return qualSpecs == Flags::Const; }

  ///\Has the stone qualifier
  bool HasStone() const { return qualSpecs & Flags::Stone; }
  ///\Has stone qualifier
  bool IsStone() const { return qualSpecs == Flags::Stone; }

  ///\Has restrict qualifier
  bool HasRestrict() const { return qualSpecs & Flags::Restrict; }
  ///\Has only the restrict qualifier
  bool IsRestrict() const { return qualSpecs == Flags::Restrict; }

  ///\Has volatile qualifier
  bool HasVolatile() const { return qualSpecs & Flags::Volatile; }
  ///\Has volatile qualifier
  bool IsVolatile() const { return qualSpecs == Flags::Volatile; }

  ///\Has volatile mutable
  bool HasMutable() const { return qualSpecs & Flags::Mutable; }
  ///\Has volatile mutable
  bool IsMutable() const { return qualSpecs == Flags::Mutable; }

private:
  ///\Clear the const flag
  void ClearConst() { qualSpecs &= ~Flags::Const; }
  ///\Add the const flag
  void AddConst() { qualSpecs |= Flags::Const; }

  ///\Clear stone qualifier
  void ClearStone() { qualSpecs &= ~Flags::Stone; }
  ///\Clear stone qualifier
  void AddStone() { qualSpecs |= Flags::Stone; }

  ///\clear the restrict qualifier
  void ClearRestrict() { qualSpecs &= ~Flags::Restrict; }
  ///\add restrict qualifier
  void AddRestrict() { qualSpecs |= Flags::Restrict; }

  ///\clear volatile qualifier
  void ClearVolatile() { qualSpecs &= ~Flags::Volatile; }
  ///\add volatile qualifier
  void AddVolatile() { qualSpecs |= Flags::Volatile; }

  ///\clear mutable qualifier
  void ClearMutable() { qualSpecs &= ~Flags::Mutable; }
  ///\add mutable qualifier
  void AddMutable() { qualSpecs |= Flags::Mutable; }

  ///\clear all volatile qualifier
  void Strip() { qualSpecs = 0; }

  ///\is valid
  bool IsValid() const;
};

} // namespace stone
#endif
