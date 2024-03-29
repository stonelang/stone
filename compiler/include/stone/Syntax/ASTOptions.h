#ifndef STONE_SYNTAX_SYNTAXOPTIONS_H
#define STONE_SYNTAX_SYNTAXOPTIONS_H

#include "stone/Basic/STDAlias.h"

#include "llvm/ADT/PointerUnion.h"

namespace stone {

class ASTOptions final {
public:
  unsigned LexingLineIndentation = 4;

public:
  ASTOptions() {}
};

class PrintASTOptions final {
public:
  PrintASTOptions() {}
};
} // namespace stone
#endif
