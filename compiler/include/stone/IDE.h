#ifndef STONE_IDE_H
#define STONE_IDE_H

#include "llvm/ADT/StringRef.h"

namespace stone {
class Parser;
class SourceFile;
class Token;
class Decl;

class CodeCompletionCallbacks {
protected:
  Parser &parser;

public:
  CodeCompletionCallbacks(Parser &parser) : parser(parser) {}
  virtual ~CodeCompletionCallbacks() {}

public:
  virtual void CompletedParseSourceFile(const SourceFile *srcFile) = 0;
  virtual void CompletedParseTopLevelDecl(const Decl *decl) = 0;
  virtual void CompletedToken(const Token *token) = 0;
};

} // namespace stone
#endif
