//===----------------------------------------------------------------------===//
///
/// \file
/// Provides info about editor placeholders, <#such as this#>.
///
//===----------------------------------------------------------------------===//

#include "stone/Basic/EditorPlaceholder.h"

#include <optional>

using namespace stone;
using namespace llvm;

// Placeholder text must start with '<#' and end with
// '#>'.
//
// Placeholder kinds:
//
// Typed:
//   'T##' display-string '##' type-string ('##' type-for-expansion-string)?
//   'T##' display-and-type-string
//
// Basic:
//   display-string
//
// NOTE: It is required that '##' is not a valid substring of display-string
// or type-string. If this ends up not the case for some reason, we can consider
// adding escaping for '##'.

std::optional<EditorPlaceholderData>
stone::parseEditorPlaceholder(StringRef PlaceholderText) {
  if (!PlaceholderText.starts_with("<#") || !PlaceholderText.ends_with("#>"))
    return std::nullopt;

  PlaceholderText = PlaceholderText.drop_front(2).drop_back(2);
  EditorPlaceholderData PHDataBasic;
  PHDataBasic.Kind = EditorPlaceholderKind::Basic;
  PHDataBasic.Display = PlaceholderText;

  if (!PlaceholderText.starts_with("T##")) {
    // Basic.
    return PHDataBasic;
  }

  // Typed.
  EditorPlaceholderData PHDataTyped;
  PHDataTyped.Kind = EditorPlaceholderKind::Typed;

  assert(PlaceholderText.starts_with("T##"));
  PlaceholderText = PlaceholderText.drop_front(3);
  size_t Pos = PlaceholderText.find("##");
  if (Pos == StringRef::npos) {
    PHDataTyped.Display = PHDataTyped.Type = PHDataTyped.TypeForExpansion =
        PlaceholderText;
    return PHDataTyped;
  }
  PHDataTyped.Display = PlaceholderText.substr(0, Pos);

  PlaceholderText = PlaceholderText.substr(Pos + 2);
  Pos = PlaceholderText.find("##");
  if (Pos == StringRef::npos) {
    PHDataTyped.Type = PHDataTyped.TypeForExpansion = PlaceholderText;
  } else {
    PHDataTyped.Type = PlaceholderText.substr(0, Pos);
    PHDataTyped.TypeForExpansion = PlaceholderText.substr(Pos + 2);
  }

  return PHDataTyped;
}

bool stone::isEditorPlaceholder(StringRef IdentifierText) {
  return IdentifierText.starts_with("<#");
}
