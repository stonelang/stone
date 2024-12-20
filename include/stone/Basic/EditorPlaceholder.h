
//===----------------------------------------------------------------------===//
///
/// \file
/// Provides info about editor placeholders, <#such as this#>.
///
//===----------------------------------------------------------------------===//

#ifndef STONE_BASIC_EDITORPLACEHOLDER_H
#define STONE_BASIC_EDITORPLACEHOLDER_H

#include "stone/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace stone {

enum class EditorPlaceholderKind {
  Basic,
  Typed,
};

struct EditorPlaceholderData {
  /// Placeholder kind.
  EditorPlaceholderKind Kind;
  /// The part that is displayed in the editor.
  StringRef Display;
  /// If kind is \c Typed, this is the type string for the placeholder.
  StringRef Type;
  /// If kind is \c Typed, this is the type string to be considered for
  /// placeholder expansion.
  /// It can be same as \c Type or different if \c Type is a typealias.
  StringRef TypeForExpansion;
};

/// Deconstructs a placeholder string and returns info about it.
/// \returns None if the \c PlaceholderText is not a valid placeholder string.
std::optional<EditorPlaceholderData>
parseEditorPlaceholder(StringRef PlaceholderText);

/// Checks if an identifier with the given text is an editor placeholder
bool isEditorPlaceholder(StringRef IdentifierText);
} // end namespace stone

#endif // SWIFT_BASIC_EDITORPLACEHOLDER_H
