#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(TreeContext &tc) : tc(tc) {}
Syntax::~Syntax() {}

template <std::size_t Len>
static bool IsNamed(const NamedDecl *namedDecl, const char (&str)[Len]) {
  Identifier *identifier = namedDecl->GetIdentifier();
  return identifier && identifier->isStr(str);
}

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

Identifier &Syntax::MakeIdentifier(llvm::StringRef name) {
  return GetTreeContext().GetIdentifier(name);
}
Module *Syntax::MakeModuleDecl(Identifier &name, bool isMainModule) {

  auto declPtr = Syntax::AllocateDeclMem<syn::Module>(GetTreeContext(),
                                                      sizeof(syn::Module));
  return ::new (declPtr) syn::Module(name, GetTreeContext());
}

FunDecl *Syntax::MakeFunDecl(SrcLoc loc, DeclContext *dc) {
  size_t size =
      sizeof(FunDecl); // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  auto declPtr = Syntax::AllocateDeclMem<FunDecl>(GetTreeContext(), size);
  return ::new (declPtr) FunDecl(loc, GetTreeContext(), dc);
}

StructDecl *Syntax::MakeStructDecl(SrcLoc loc, DeclContext *dc) {
  size_t size = sizeof(StructDecl);

  auto declPtr = Syntax::AllocateDeclMem<StructDecl>(GetTreeContext(), size);
  // return ::new (declPtr) StructDecl(loc, GetTreeContext(), dc);
  return nullptr;
}