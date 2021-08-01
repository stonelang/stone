#ifndef STONE_SEMANTICS_TYPECHECKERPIPELINE_H
#define STONE_SEMANTICS_TYPECHECKERPIPELINE_H

#include "stone/Basic/PipelineListener.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {
class Decl;
class Stmt;
class Expr;
class SyntaxFile;

} // namespace syn
class TypeCheckerListener : public PipelineListener {
public:
  TypeCheckerListener()
      : PipelineListener(PipelineListenerKind::TypeChecking) {}

public:
  virtual void OnDeclTypeChecked(syn::Decl *decl, bool isTopLvel = false) {}
  virtual void OnStmtTypeChecked(syn::Stmt *stmt) {}
  virtual void OnExprTypeChecked(syn::Expr *expr) {}

public:
  virtual void OnTypeCheckCompleted(syn::SyntaxFile *syntaxFile) {}

public:
};
} // namespace stone
#endif
