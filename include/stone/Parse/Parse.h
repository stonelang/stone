#ifndef STONE_PARSE_PARSE_H
#define STONE_PARSE_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class SyntaxListener;

namespace syn {
class Syntax;
class SyntaxFile;

void ParseSyntaxFile(SyntaxFile &sf, Syntax &syntax, SyntaxListener *sp = nullptr);

} // namespace syn
} // namespace stone
#endif
