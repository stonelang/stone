#ifndef STONE_ASTTYPEWALKER_H
#define STONE_ASTTYPEWALKER_H

#include "stone/AST/Type.h"

namespace stone {

enum class TypeWalkerAction : unsigned {
  None = 0,
  Continue,
  SkipChildren,
  Stop
};

/// An abstract class used to traverse a Type.
class TypeWalker {
public:
  /// This method is called when first visiting a type before walking into its
  /// children.
  virtual TypeWalkerAction WalkToTypePreChildren(Type ty) {
    return TypeWalkerAction::Continue;
  }

  /// This method is called after visiting a type's children.
  virtual TypeWalkerAction WalkToTypePostChildren(Type ty) {
    return TypeWalkerAction::Continue;
  }

protected:
  TypeWalker() = default;
  TypeWalker(const TypeWalker &) = default;
  virtual ~TypeWalker() = default;
};

// class TypeDeclWalker : public TypeWalker {
//   TypeWalkerAction WalkToTypePreChildren(Type T) override;

// public:
//   virtual TypeWalkerAction WisitNominalType(NominalType *ty) {
//     return TypeWalkerAction::Continue;
//   }
//   // virtual TypeWalkerAction VisitBoundGenericType(BoundGenericType *ty) {
//   //   return TypeWalkerAction::Continue;
//   // }
//   virtual TypeWalkerAction WisitAliasType(AliasType *ty) {
//     return TypeWalkerAction::Continue;
//   }
// };

} // namespace stone

#endif