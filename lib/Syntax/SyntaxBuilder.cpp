#include "stone/Syntax/SyntaxBuilder.h"

#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

// Decl
DeclSyntaxBuilder::DeclSyntaxBuilder(Syntax &syntax) : SyntaxBuilder(syntax) {}

StmtSyntaxBuilder::StmtSyntaxBuilder(Syntax &syntax) : SyntaxBuilder(syntax) {}
// Block
BlockStmtSyntaxBuilder::BlockStmtSyntaxBuilder(Syntax &syntax)
    : StmtSyntaxBuilder(syntax) {}

// Fun
FunDeclSyntaxBuilder::FunDeclSyntaxBuilder(Syntax &syntax)
    : DeclSyntaxBuilder(syntax), BlockStmtSyntaxBuilder(syntax) {}

FunDecl *FunDeclSyntaxBuilder::Build() { return nullptr; }

void FunDeclSyntaxBuilder::WithFunKeyword() {}

// Struct
StructDeclSyntaxBuilder::StructDeclSyntaxBuilder(Syntax &syntax)
    : DeclSyntaxBuilder(syntax), BlockStmtSyntaxBuilder(syntax) {}

void StructDeclSyntaxBuilder::WithStructKeyword() {}