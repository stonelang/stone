#ifndef STONE_SEMANTICS_TYPECHECKERPIPELINE_H
#define STONE_SEMANTICS_TYPECHECKERPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {
class Decl;
class Stmt;
class Expr;
} // namespace syn
class TypeCheckerPipeline : public Pipeline {
public:
  TypeCheckerPipeline() : Pipeline(PipelineType::TypeCheck) {}

public:
};
} // namespace stone
#endif
