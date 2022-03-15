#ifndef STONE_CORE_TOKENKIND_H
#define STONE_CORE_TOKENKIND_H

#include "stone/Core/LLVM.h"

namespace {
enum { TOKON = 0x1, TOKOFF = 0x2, TOKRSV = 0x4 };
}
namespace stone {
namespace tk {
enum class Kind : uint8_t {
#define TOKEN(X) X,
#include "stone/Core/TokenKind.def"
  MAX
};
/// Check whether a token kind is known to have any specific text content.
/// e.g., tk::l_paren has determined text however tk::Kind::identifier doesn't.
bool IsTokenTextDetermined(tk::Kind kind);

/// If a token kind has determined text, return the text; otherwise assert.
const char *GetName(tk::Kind kind);

void DumpTokenKind(llvm::raw_ostream &os, tk::Kind kind);
} // namespace tk
} // namespace stone
#endif
