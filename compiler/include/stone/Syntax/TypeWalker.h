#ifndef STONE_SYNTAX_TYPEWALKER_H
#define STONE_SYNTAX_TYPEWALKER_H

#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"

#include <string>

namespace stone {
namespace syn {

enum class TypeWalkerAction { None = 0, Continue, SkipChildren, Stop };

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
} // namespace syn
} // namespace stone