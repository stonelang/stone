#ifndef STONE_COMPILE_CHECKERPIPELINE_H
#define STONE_COMPILE_CHECKERPIPELINE_H

#include "stone/Core/PipelineEngine.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Stmt.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CheckerPipeline : public Pipeline {

public:
  CheckerPipeline() : Pipeline(PipelineType::Check) {}

public:
  llvm::StringRef GetName() override { return "Check"; }

public:
  virtual void OnDeclChecked(const syn::Decl *decl) = 0;
};

} // namespace stone
#endif
