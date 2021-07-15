#include "stone/Syntax/DeclName.h"

using namespace stone;
using namespace stone::syn;

int DeclName::Compare(DeclName lhs, DeclName rhs) { return 0; }

void DeclName::Print(llvm::raw_ostream &os,
                     const PrintingPolicy &policy) const {}

void DeclName::Dump() const {}
