#ifndef STONE_DIAG_DIAGNOSTICTEXTPARSER_H
#define STONE_DIAG_DIAGNOSTICTEXTPARSER_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
class SrcMgr;
namespace diags {

enum class TextTokenKind {
  Identifier,
  Select,
};

class TextToken {
public:
};
/// May want to think about creating a source buffer using SrcMgr
// and treating this like a source file to parse with tokens and identifiers
// TextParserToken TextParserTokenKind
class TextParser final {
  unsigned BufferID;
  SrcMgr &SM;

private:
  TextParser(unsigned BufferID, SrcMgr &SM);
  void Parse();
  void ParseIdentifier();
  void ParseSelect();
};

} // namespace diags
} // namespace stone
#endif