#ifndef STONE_SYNTAX_SYNTAXFACTORY_H
#define STONE_SYNTAX_SYNTAXFACTORY_H

#include "stone/Syntax/Module.h"

#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace syn {
class Decl;
class DeclContext;
class SyntaxContext;
class FunDecl;
class MemberFunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class SwitchStmt;
class Expr;
class Syntax;
class SyntaxFile;
struct DeclNameInfo;

} // namespace syn

namespace syn {
template <typename DeclTy, typename AllocatorTy>
void *AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
                      bool extraSace = false);

struct FunDeclFactory final {
  static FunDecl *Create(DeclNameInfo &nameInfo, SyntaxContext &sc,
                         TypeRep *result, DeclContext *parent);
};

struct SyntaxFileFactory final {
  static SyntaxFile *Create(SyntaxFileKind kind, unsigned srcID, Module &owner,
                            SyntaxContext &sc, bool isPrimary = false);
};

StructDecl *MakeStructDecl(DeclName name, SrcLoc loc, SyntaxContext &sc,
                           DeclContext *parent = nullptr);

Module *MakeModuleDecl(Identifier *name, SyntaxContext &sc,
                       bool isMainModule = false);

void VerifyDecl(Decl *d);
void VerifyFunDecl(Decl *d);

// template <std::size_t Len>
// bool IsNamedDecl(const NamedDecl *namedDecl, const char (&str)[Len]) {
//   Identifier *identifier = namedDecl->GetIdentifier();
//   return identifier && identifier->isStr(str);
// }

//== Statements ==/
BraceStmt *MakeBraceStmt(SrcLoc lbloc, llvm::ArrayRef<SyntaxNode> elements,
                         SrcLoc rbloc, SyntaxContext &sc,
                         llvm::Optional<bool> implicit = llvm::None);

} // namespace syn

} // namespace stone

#endif
