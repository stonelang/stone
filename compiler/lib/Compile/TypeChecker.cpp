#include "stone/Sem/TypeChecker.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;
using namespace stone::sem;
using namespace stone::syn;

TypeChecker::TypeChecker(SyntaxContext &sc, TypeCheckerOptions &typeCheckerOpts,
                         TypeCheckerListener *pipeline)
    : sc(sc), typeCheckerOpts(typeCheckerOpts), pipeline(pipeline) {

  stats.reset(new TypeCheckerStats(*this));
  sc.GetLangContext().GetStatEngine().Register(stats.get());
}

TypeChecker::~TypeChecker() {}

void TypeCheckerStats::Print(ColorStream &stream) {}
