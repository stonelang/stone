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

class SyntaxPipeline : public Pipeline {

public:
  SyntaxPipeline() : Pipeline(PipelineType::Syntax) {}

public:
  llvm::StringRef GetName() override { return "Parse"; }

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnTopDecl(const syn::Decl *decl) {}
  virtual void OnDecl(const syn::Decl *decl) {}

  virtual void OnStmt(const syn::Stmt *stmt) {}
  virtual void OnExpr(const syn::Expr *expr) {}

  virtual void Listen(const syn::SyntaxFile *expr) {}

public:
  virtual void OnToken(const syn::Token *token) {}
};
} // namespace stone
#endif
