#ifndef STONE_PARSE_PARSE_H
#define STONE_PARSE_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class SyntaxListener;

namespace syn {
class SyntaxFile;
class SyntaxContext;

void Parse(SyntaxFile &sf, SyntaxContext &sc, SyntaxListener *sp = nullptr);

} // namespace syn
} // namespace stone
#endif
