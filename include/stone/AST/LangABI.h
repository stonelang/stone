#ifndef STONE_AST_LANGABI_H
#define STONE_AST_LANGABI_H

#include "stone/Basic/STDAlias.h"

namespace stone {

enum class ConstructorKind : UInt8 {
  None = 0,
  Complete,       ///< Complete object ctor
  Base,           ///< Base object ctor
  Comdat,         ///< The COMDAT used for ctors
  CopyingClosure, ///< Copying closure variant of a ctor
  DefaultClosure, ///< Default closure variant of a ctor
};

enum class DesctructorKind : UInt8 {
  None = 0,
  Complete, ///< Complete object dtor
  Base,     ///< Base object dtor
  Deleting, ///< Deleting dtor
  Comdat    ///< The COMDAT used for dtors
};

class LangABI final {
public:
};

} // namespace stone

#endif
