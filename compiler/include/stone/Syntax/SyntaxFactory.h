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
struct DeclNameContext;

} // namespace syn

namespace syn {
template <typename DeclTy, typename AllocatorTy>
void *AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
                      bool extraSace = false);

// class SyntaxFactory final {
// public:
//   SyntaxFactory() = delete;

// public:
//   static FunDecl *CreateFunDecl(DeclNameContext &dnc,
//   SyntaxContext &sc,
//                                 TypeRep *result, DeclContext *parent);

// public:
//   static SyntaxFile *CreateSyntaxFile(SyntaxFileKind kind, unsigned srcID,
//                                       Module &owner, SyntaxContext &sc,
//                                       bool isPrimary = false);

// public:
//   static Module *CreateModuleDecl(Identifier *name, SyntaxContext &sc,
//                                   bool isMainModule = false);

// public:
//   static BraceStmt *CreateBraceStmt(SrcLoc lbloc,
//                                     llvm::ArrayRef<SyntaxNode> elements,
//                                     SrcLoc rbloc, SyntaxContext &sc,
//                                     llvm::Optional<bool> implicit =
//                                     llvm::None);
// };

struct VarDeclFactory final {
  static VarDecl *Create(SyntaxContext &sc);
};

struct FunDeclFactory final {
  static FunDecl *Create(DeclNameContext &dnc, SyntaxContext &sc,
                         TypeRep *resultRep, DeclContext *parent);

  static FunDecl *CreateImplicit(DeclNameContext &dnc, SyntaxContext &sc,
                                 QualType *returnType, DeclContext *parent);
};

struct SyntaxFileFactory final {
  static SyntaxFile *Create(SyntaxFileKind kind, unsigned srcID, Module &owner,
                            SyntaxContext &sc, bool isPrimary = false);
};

struct StructDeclFactory final {
  StructDecl *Create(DeclName name, SrcLoc loc, SyntaxContext &sc,
                     DeclContext *parent = nullptr);
};

struct ModuleDeclFactory final {
  static Module *Create(Identifier *name, SyntaxContext &sc,
                        bool isMainModule = false);
};

// template <std::size_t Len>
// bool IsNamedDecl(const NameableDecl *nameableDecl, const char (&str)[Len]) {
//   Identifier *identifier = namedDecl->GetIdentifier();
//   return identifier && identifier->isStr(str);
// }

struct BraceStmtFactory {
  //== Statements ==/
  static BraceStmt *Create(SrcLoc lbloc, llvm::ArrayRef<SyntaxNode> elements,
                           SrcLoc rbloc, SyntaxContext &sc,
                           llvm::Optional<bool> implicit = llvm::None);
};

} // namespace syn

} // namespace stone

#endif
