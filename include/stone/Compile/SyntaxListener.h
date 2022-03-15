#ifndef STONE_PARSE_PARSERPIPELINE_H
#define STONE_PARSE_PARSERPIPELINE_H

#include "stone/Core/PipelineListener.h"
#include "stone/Core/Token.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class LexerListener : public PipelineListener {
public:
  LexerListener() : PipelineListener(PipelineListenerKind::Lexing) {}

public:
  virtual void OnToken(const syn::Token *token) {}
};

class SyntaxListener : public PipelineListener {

public:
  SyntaxListener() : PipelineListener(PipelineListenerKind::Parsing) {}

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnDecl(const syn::Decl *decl, bool isTopLevel = false) {}
  virtual void OnStmt(const syn::Stmt *stmt) {}
  virtual void OnExpr(const syn::Expr *expr) {}
  virtual void OnToken(const syn::Token *token) {}

public:
  virtual void OnParseError(syn::SyntaxFile *syntaxFile) {}
  virtual void OnParseCompleted(syn::SyntaxFile *syntaxFile) {}
  virtual void OnSyntaxFileCompleted(syn::SyntaxFile *syntaxFile) {}
};

} // namespace stone
#endif
