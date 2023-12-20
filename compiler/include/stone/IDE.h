#ifndef STONE_IDE_H
#define STONE_IDE_H

#include "llvm/ADT/StringRef.h"

namespace stone {
class Parser;
class ModuleDecl;
class Decl;
class Stmt;
class Expr;
class Token;
class SourceFile;
class ModuleFile;

class CodeCompletionCallbacks {
protected:
  Parser &parser;

public:
  CodeCompletionCallbacks(Parser &parser) : parser(parser) {}

public:
  virtual void CompletedParseSourceFile(SourceFile *srcFile) = 0;
};

} // namespace stone
#endif
