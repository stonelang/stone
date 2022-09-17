#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"

using namespace stone;
using namespace stone::syn;

template <typename DeclTy, typename AllocatorTy>
void *syn::AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
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

FunDecl *FunDeclFactory::Create(DeclNameInfo &nameInfo, SyntaxContext &sc,
                                TypeRep *result, DeclContext *parent) {
  size_t size = sizeof(FunDecl);
  // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  auto memPtr = syn::AllocateDeclMem<FunDecl>(sc, size);
  return ::new (memPtr)
      FunDecl(DeclKind::Fun, nameInfo.GetName(), nameInfo.GetNameLoc(),
              nameInfo.GetSpecialNameLoc(), parent);
}

StructDecl *StructDeclFactory::Create(DeclName name, SrcLoc loc,
                                      SyntaxContext &sc, DeclContext *dc) {
  size_t size = sizeof(StructDecl);
  auto declPtr = syn::AllocateDeclMem<StructDecl>(sc, size);
  // return ::new (declPtr) StructDecl(loc, GetSyntaxContext(), dc);
  return nullptr;
}

Module *ModuleDeclFactory::Create(Identifier *name, SyntaxContext &sc,
                                  bool isMainModule) {
  auto declPtr = syn::AllocateDeclMem<syn::Module>(sc, sizeof(syn::Module));
  return ::new (declPtr) syn::Module(name, sc);
}
