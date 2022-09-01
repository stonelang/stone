#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

template <typename DeclTy, typename AllocatorTy>
void *Syntax::AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
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

Module *Syntax::MakeModuleDecl(Identifier *name, bool isMainModule) {
  auto declPtr = Syntax::AllocateDeclMem<syn::Module>(GetSyntaxContext(),
                                                      sizeof(syn::Module));
  return ::new (declPtr) syn::Module(name, GetSyntaxContext());
}

FunDecl *Syntax::MakeFunDecl(DeclName name, SrcLoc nameLoc,
                             DeclContext *parent) {
  size_t size =
      sizeof(FunDecl); // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  auto declPtr = Syntax::AllocateDeclMem<FunDecl>(GetSyntaxContext(), size);
  return ::new (declPtr) FunDecl(name, nameLoc, parent);
}

StructDecl *Syntax::MakeStructDecl(DeclName name, SrcLoc loc, DeclContext *dc) {
  size_t size = sizeof(StructDecl);

  auto declPtr = Syntax::AllocateDeclMem<StructDecl>(GetSyntaxContext(), size);
  // return ::new (declPtr) StructDecl(loc, GetSyntaxContext(), dc);
  return nullptr;
}
