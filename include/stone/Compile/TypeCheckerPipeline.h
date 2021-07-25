#ifndef STONE_CODEANALYSIS_CHECKERPIPELINE_H
#define STONE_CODEANALYSIS_CHECKERPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Stmt.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class TypeCheckerPipeline : public Pipeline {

public:
  TypeCheckerPipeline() : Pipeline(PipelineType::TypeCheck) {}

public:
  llvm::StringRef GetName() override { return "TypeCheck"; }

public:
  virtual void OnDecl(const syn::Decl *decl) = 0;
};

} // namespace stone
#endif
