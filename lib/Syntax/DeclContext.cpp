#include "stone/Syntax/DeclContext.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/TreeContext.h"

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

DeclContext::DeclContext(DeclContextKind dcTy, DeclKind dTy,
                         DeclContext *parent)
    : dcTy(dcTy), dTy(dTy), parent(parent) {
  // declContextBits.DeclKind = ty;
}

TreeContext &DeclContext::GetTreeContext() const {
  return GetParentModule()->GetTreeContext();
}

Module *DeclContext::GetParentModule() const {
  const DeclContext *dc = this;
  // TODO:
  return nullptr;
  // while (!dc->IsModuleContext()){
  //   dc = dc->GetParent();
  // }
  // return const_cast<Module *>(cast<Module>(dc));
}
