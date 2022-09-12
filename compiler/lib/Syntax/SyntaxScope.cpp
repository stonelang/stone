#include "stone/Syntax/SyntaxScope.h"

using stone::syn::SyntaxScope;

SyntaxScope::SyntaxScope(SyntaxScopeKind kind, DiagnosticEngine &diags,
                         SyntaxScope *parent)
    : kind(kind), diags(diags), parent(parent) {}
SyntaxScope::~SyntaxScope() {}
