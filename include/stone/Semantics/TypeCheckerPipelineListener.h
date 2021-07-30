#ifndef STONE_SEMANTICS_TYPECHECKERPIPELINE_H
#define STONE_SEMANTICS_TYPECHECKERPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {
class Decl;
class Stmt;
class Expr;
class SyntaxFile;

} // namespace syn
class TypeCheckerPipelineListener : public PipelineListener {
public:
  TypeCheckerPipelineListener()
      : PipelineListener(PipelineListenerKind::TypeCheck) {}

public:

  virtual void OnTopDeclTypeChecked(syn::Decl* decl) {}
  virtual void OnStmtTypeChecked(syn::Stmt* stmt) {}
  virtual void OnExprTypeChecked(syn::Expr* expr) {}

  virtual void OnSyntaxFileTypeChecked(syn::SyntaxFile *syntaxFile) {}
  virtual void OnModuleTypeChecked(syn::SyntaxFile *syntaxFile) {}

public:
};
} // namespace stone
#endif
