#ifndef STONE_CHECK_TYPECHECKERPIPELINE_H
#define STONE_CHECK_TYPECHECKERPIPELINE_H

#include "stone/Basic/PipelineListener.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {
class Decl;
class Stmt;
class Expr;
class Module;
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
  virtual void OnTypeCheckError() {}
  virtual void OnSyntaxFileTypeChecked(syn::SyntaxFile *syntaxFile) {}
  virtual void OnModuleTypeChecked(syn::Module *mod) {}

public:
};
} // namespace stone
#endif
