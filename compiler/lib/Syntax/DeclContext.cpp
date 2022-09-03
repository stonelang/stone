#include "stone/Syntax/DeclContext.h"

#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using namespace stone::syn;

DeclContext::DeclContext(DeclContextKind declContextKind, DeclContext *parent)
    : declContextKind(declContextKind), parent(parent) {}

SyntaxContext &DeclContext::GetSyntaxContext() const {
  return GetParentModule()->GetSyntaxContext();
}

syn::Module *DeclContext::GetParentModule() const {
  const DeclContext *dc = this;
  // TODO:
  return nullptr;
  // while (!dc->IsModuleContext()){
  //   dc = dc->GetParent();
  // }
  // return const_cast<Module *>(cast<Module>(dc));
}

DeclContext *Decl::GetDeclContextForModule() const {
  if (auto module = dyn_cast<syn::Module>(this)) {
    return const_cast<syn::Module *>(module);
  }
  return nullptr;
}
