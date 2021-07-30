#ifndef STONE_PARSE_PARSERPIPELINE_H
#define STONE_PARSE_PARSERPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "stone/Basic/Token.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {


class LexerPipelineListener : public PipelineListener {
public:
  LexerPipelineListener() : PipelineListener(PipelineListenerKind::Lexing) {}

public:
  virtual void OnToken(const syn::Token *token) {}
};

class SyntaxPipelineListener : public PipelineListener {

public:
  SyntaxPipelineListener() : PipelineListener(PipelineListenerKind::Parsing) {}

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnTopDecl(const syn::Decl *decl) {}
  virtual void OnDecl(const syn::Decl *decl) {}

  virtual void OnStmt(const syn::Stmt *stmt) {}
  virtual void OnExpr(const syn::Expr *expr) {}

  virtual void OnSyntaxFileParsed(syn::SyntaxFile *syntaxFile) {}

public:
  virtual void OnToken(const syn::Token *token) {}
};

} // namespace stone
#endif
