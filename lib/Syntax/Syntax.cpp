#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(std::unique_ptr<SyntaxContext> sc) : sc(std::move(sc)) {}

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
  return GetSyntaxContext().GetIdentifier(name);
}
Module *Syntax::MakeModuleDecl(Identifier &name, bool isMainModule) {

  auto declPtr = Syntax::AllocateDeclMem<syn::Module>(GetSyntaxContext(),
                                                      sizeof(syn::Module));
  return ::new (declPtr) syn::Module(name, GetSyntaxContext());
}

FunDecl *Syntax::MakeFunDecl(SrcLoc loc, DeclContext *dc) {
  size_t size =
      sizeof(FunDecl); // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  auto declPtr = Syntax::AllocateDeclMem<FunDecl>(GetSyntaxContext(), size);
  return ::new (declPtr) FunDecl(loc, GetSyntaxContext(), dc);
}

StructDecl *Syntax::MakeStructDecl(SrcLoc loc, DeclContext *dc) {
  size_t size = sizeof(StructDecl);

  auto declPtr = Syntax::AllocateDeclMem<StructDecl>(GetSyntaxContext(), size);
  // return ::new (declPtr) StructDecl(loc, GetSyntaxContext(), dc);
  return nullptr;
}