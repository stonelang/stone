#ifndef STONE_PARSE_PARSING_H
#define STONE_PARSE_PARSING_H

#include "stone/AST/DeclState.h"
#include "stone/AST/TypeState.h"
#include "stone/Parse/Parser.h"

#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Timer.h"

namespace stone {

class ParsingDeclState final : public DeclState {
  Parser &parser;

public:
  ParsingDeclState(Parser &parser) : DeclState(parser.GetASTContext()) {}
};
} // namespace stone
#endif
