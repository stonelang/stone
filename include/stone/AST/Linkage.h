#ifndef STONE_AST_LINKAGE_H
#define STONE_AST_LINKAGE_H

#include "stone/Basic/Basic.h"

namespace stone {
/// Describes the different kinds of linkage
/// (C++ [basic.link], C99 6.2.2) that an entity may have.
enum class LinkageKind : uint8 {
  /// No linkage, which means that the entity is unique and
  /// can only be referred to from within its scope.
  None = 0,

  /// Internal linkage, which indicates that the entity can
  /// be referred to from within the translation unit (but not other
  /// translation units).
  InternalLinkage,

  /// External linkage within a unique namespace.
  ///
  /// From the language perspective, these entities have external
  /// linkage. However, since they reside in an anonymous namespace,
  /// their names are unique to this translation unit, which is
  /// equivalent to having internal linkage from the code-generation
  /// point of view.
  UniqueExternalLinkage,

  /// No linkage according to the standard, but is visible from other
  /// translation units because of types defined in a inline function.
  VisibleNoLinkage,

  /// Internal linkage according to the Modules TS, but can be referred
  /// to from other translation units indirectly through inline functions and
  /// templates in the module interface.
  ModuleInternalLinkage,

  /// Module linkage, which indicates that the entity can be referred
  /// to from other translation units within the same module, and indirectly
  /// from arbitrary other translation units through inline functions and
  /// templates in the module interface.
  ModuleLinkage,

  /// External linkage, which indicates that the entity can
  /// be referred to from other translation units.
  ExternalLinkage
};
/// A more specific kind of linkage than enum Linkage.
///
/// This is relevant to CodeGen and Syntax file reading.
enum class GlobalValueLinkageKind : uint8 {
  None = 0,
  Internal,
  External,
  DiscardableODR,
  StrongExternal,
  StrongODR
};

} // namespace stone

#endif