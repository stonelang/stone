#ifndef STONE_CODEANALYSIS_PARSE_H
#define STONE_CODEANALYSIS_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class PipelineEngine;

namespace syn {
class Syntax;
class SyntaxFile;
void ParseSyntaxFile(syn::SyntaxFile &sf, Syntax &syntax,
                     PipelineEngine *pe = nullptr);

} // namespace syn
} // namespace stone
#endif
