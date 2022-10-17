#include "stone/Syntax/TypeRep.h"
#include "stone/Syntax/SyntaxWalker.h"

using namespace stone;
using namespace stone::syn;

void TypeRep::Print(raw_ostream &os,
                    const PrintSyntaxOptions &printOpts) const {}

SpecifierTypeRep *SpecifierTypeRep::Create(SyntaxContext &sc) {}

QualifierTypeRep *QualifierTypeRep::Create(TypeRep *type, SyntaxContext &sc) {}