#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/DeclSpecifier.h"
#include "stone/Syntax/Module.h"

using namespace stone;

template <typename DeclTy, typename AllocatorTy>
void *Decl::AllocateMemory(AllocatorTy &allocatorTy, size_t baseSize,
                           bool extraSace) {
  static_assert(alignof(DeclTy) >= sizeof(void *),
                "A pointer must fit in the alignment of the DeclTy!");

  size_t size = baseSize;
  if (extraSace) {
    size += alignof(DeclTy);
  }
  void *mem = allocatorTy.Allocate(size, alignof(DeclTy));
  if (extraSace)
    mem = reinterpret_cast<char *>(mem) + alignof(DeclTy);
  return mem;
}

FunDecl *FunDecl::Create(DeclSpecifierCollector &collector,
                         ASTContext &astContext, DeclContext *parent) {
  size_t size = sizeof(FunDecl);
  // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  auto memPtr = Decl::AllocateMemory<FunDecl>(astContext, size);
  auto funDecl = ::new (memPtr) FunDecl(
      DeclKind::Fun, collector.GetFunctionSpecifierCollector().GetFunLoc(),
      collector.GetDeclNameCollector().GetName(),
      collector.GetDeclNameCollector().GetLoc(),
      collector.GetTypeSpecifierCollector().GetType(), parent);

  // call apply on the collector
  funDecl->SetAccessLevel(
      collector.GetAccessSpecifierCollector().GetAccessLevel());
  return funDecl;
}
FunDecl *FunDecl::CreateImplicit(DeclSpecifierCollector &collector,
                                 ASTContext &sc, DeclContext *parent) {

  return nullptr;
}

StructDecl *StructDecl::Create(DeclName name, SrcLoc loc,
                               ASTContext &astContext, DeclContext *dc) {

  size_t size = sizeof(StructDecl);
  auto declPtr = Decl::AllocateMemory<StructDecl>(astContext, size);
  // return ::new (declPtr) StructDecl(loc, GetASTContext(), dc);
  return nullptr;
}

ModuleDecl *ModuleDecl::Create(Identifier name, ASTContext &astContext) {
  size_t size = sizeof(ModuleDecl);
  auto declPtr = Decl::AllocateMemory<ModuleDecl>(astContext, size);
  return ::new (declPtr) ModuleDecl(name, astContext);
}

ModuleDecl *ModuleDecl::CreateMainModule(Identifier name,
                                         ASTContext &astContext) {
  auto mainModuleDecl = ModuleDecl::Create(name, astContext);
  mainModuleDecl->Bits.ModuleDecl.IsMainModule = true;
  return mainModuleDecl;
}

VarDecl *VarDecl::Create(ASTContext &astContext) {
  // auto declPtr = stone::AllocateMem<VarDecl>(astContext,
  // sizeof(VarDecl)); return ::new (declPtr) VarDecl(astContext);
  return nullptr;
}
