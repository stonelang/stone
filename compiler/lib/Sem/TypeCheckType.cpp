#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxWalker.h"
#include "stone/Syntax/TypeVisitor.h"

using namespace stone::sem;
using namespace stone::syn;

class TypeChecking final : public TypeVisitor<TypeChecking>,
                           public SyntaxWalker {
public:
};

void TypeChecker::CheckType() {}