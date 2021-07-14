#ifndef STONE_ANALYZE_PARSE_H
#define STONE_ANALYZE_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class PipelineEngine;

namespace syn {
class Syntax;
class SyntaxFile;
} // namespace syn
void ParseSyntaxFile(syn::SyntaxFile &sf, syn::Syntax &syntax, bool check,
                     PipelineEngine *pe = nullptr);
} // namespace stone
#endif
