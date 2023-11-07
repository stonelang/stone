#include "stone/Compile/TypeChecker.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/TypeCheckerOptions.h"
#include "stone/CodeCompletionListener.h"

using namespace stone;
using namespace stone::ast;
using namespace stone::typecheck;

TypeChecker::TypeChecker(ASTContext &sc, TypeCheckerOptions &typeCheckerOpts,
                         TypeCheckerListener *pipeline)
    : sc(sc), typeCheckerOpts(typeCheckerOpts), pipeline(pipeline) {

  stats.reset(new TypeCheckerStats(*this));
  sc.GetLang().GetStats().Register(stats.get());
}

TypeChecker::~TypeChecker() {}

void TypeCheckerStats::Print(ColorStream &stream) {}
