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
  virtual void OnSyntaxFileTypeChecked(syn::SyntaxFile *syntaxFile) = 0;
  virtual void OnModuleTypeChecked(syn::SyntaxFile *syntaxFile) = 0;

public:
};
} // namespace stone
#endif
