#include "stone/Syntax/SyntaxScope.h"

using stone::syn::SyntaxScope;

SyntaxScope::SyntaxScope(SyntaxScopeKind kind, SyntaxScope *parent)
    : kind(kind), parent(parent) {}
SyntaxScope::~SyntaxScope() {}
