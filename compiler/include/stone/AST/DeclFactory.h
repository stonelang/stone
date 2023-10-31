#ifndef STONE_ASTFACTORY_H
#define STONE_ASTFACTORY_H

#include "stone/AST/DeclCollector.h"
#include "stone/AST/Module.h"

#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace ast {
class Decl;
class DeclContext;
class ASTContext;
class FunDecl;
class MemberFunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class SwitchStmt;
class Expr;
class AST;
class ASTFile;
struct DeclNameContext;

} // namespace ast

namespace ast {
template <typename DeclTy, typename AllocatorTy>
void *AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
                      bool extraSace = false);

struct DeclFactory final {

  static VarDecl *MakeVarDecl(ASTContext &sc);

  static FunDecl *MakeFunDecl(DeclCollector &collector, ASTContext &sc,
                              DeclContext *parent);
  static FunDecl *MakeFunDeclImplicit(DeclCollector &collector, ASTContext &sc,
                                      DeclContext *parent);

  static StructDecl *MakeStructDecl(DeclName name, SrcLoc loc, ASTContext &sc,
                                    DeclContext *parent = nullptr);

  static InterfaceDecl *MakeInterfaceDecl(DeclName name, SrcLoc loc,
                                          ASTContext &sc,
                                          DeclContext *parent = nullptr);

  static EnumDecl *MakeEnumDecl(DeclName name, SrcLoc loc, ASTContext &sc,
                                DeclContext *parent = nullptr);

  // TODO -- This is better
  static ModuleDecl *MakeModuleDecl(Identifier name, ASTContext &sc,
                                    bool isMainModule = false);

  // template <std::size_t Len>
  // bool IsNamedDecl(const NameableDecl *nameableDecl, const char (&str)[Len])
  // {
  //   Identifier *identifier = namedDecl->GetIdentifier();
  //   return identifier && identifier->isStr(str);
  // }
};

} // namespace ast
} // namespace stone

#endif
