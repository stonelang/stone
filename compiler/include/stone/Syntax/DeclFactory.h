#ifndef STONE_SYNTAX_SYNTAXFACTORY_H
#define STONE_SYNTAX_SYNTAXFACTORY_H

#include "stone/Syntax/DeclCollector.h"
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
class SyntaxFile;
struct DeclNameContext;

} // namespace syn

namespace syn {
template <typename DeclTy, typename AllocatorTy>
void *AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
                      bool extraSace = false);

struct DeclFactory final {

  static VarDecl *MakeVarDecl(SyntaxContext &sc);

  static FunDecl *MakeFunDecl(DeclCollector &collector, SyntaxContext &sc,
                              DeclContext *parent);
  static FunDecl *MakeFunDeclImplicit(DeclCollector &collector,
                                      SyntaxContext &sc, DeclContext *parent);

  static StructDecl *MakeStructDecl(DeclName name, SrcLoc loc,
                                    SyntaxContext &sc,
                                    DeclContext *parent = nullptr);

  static InterfaceDecl *MakeInterfaceDecl(DeclName name, SrcLoc loc,
                                          SyntaxContext &sc,
                                          DeclContext *parent = nullptr);

  static EnumDecl *MakeEnumDecl(DeclName name, SrcLoc loc, SyntaxContext &sc,
                                DeclContext *parent = nullptr);

  // TODO -- This is better
  static ModuleDecl *MakeModuleDecl(Identifier name, SyntaxContext &sc,
                                    bool isMainModule = false);

  // template <std::size_t Len>
  // bool IsNamedDecl(const NameableDecl *nameableDecl, const char (&str)[Len])
  // {
  //   Identifier *identifier = namedDecl->GetIdentifier();
  //   return identifier && identifier->isStr(str);
  // }
};

} // namespace syn
} // namespace stone

#endif
