#ifndef STONE_BASIC_TOKENKIND_H
#define STONE_BASIC_TOKENKIND_H

#include "stone/Basic/LLVM.h"
 
namespace stone {

enum class tok : uint8_t {
#define TOKEN(X) X,
#include "stone/Basic/TokenKind.def"
  LAST
};
/// Check whether a token kind is known to have any specific text content.
/// e.g., tk::l_paren has determined text however tok::identifier doesn't.
bool IsTokenTextDetermined(tok kind);

/// If a token kind has determined text, return the text; otherwise assert.
const char *GetTokenNameByKind(tok kind);

void DumpTokenKind(llvm::raw_ostream &os, tok kind);

} // namespace stone
#endif
