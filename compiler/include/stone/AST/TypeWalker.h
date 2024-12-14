#ifndef STONE_AST_TYPEWALKER_H
#define STONE_AST_TYPEWALKER_H

#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeKind.h"
#include "stone/Basic/Memory.h"

#include <string>

namespace stone {
enum class TypeWalkerAction { None = 0, Continue, SkipChildren, Stop };

class QualType;

/// An abstract class used to traverse a Type.
class TypeWalker {
public:
  /// This method is called when first visiting a type before walking into its
  /// children.
  virtual TypeWalkerAction WalkToTypePreChildren(QualType ty) {
    return TypeWalkerAction::Continue;
  }

  /// This method is called after visiting a type's children.
  virtual TypeWalkerAction WalkToTypePostChildren(QualType ty) {
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