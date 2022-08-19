#include "stone/Syntax/Syntax.h"

#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(std::unique_ptr<SyntaxContext> sc) : sc(std::move(sc)) {}

Syntax::~Syntax() {}

template <std::size_t Len>
static bool IsNamed(const NamedDecl *namedDecl, const char (&str)[Len]) {
  Identifier *identifier = namedDecl->GetIdentifier();
  return identifier && identifier->isStr(str);
}

Identifier &Syntax::MakeIdentifier(llvm::StringRef name) {
  return GetSyntaxContext().GetIdentifier(name);
}
