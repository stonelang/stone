#include "stone/Syntax/DeclContext.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/SystemOptions.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"
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

DeclContext::DeclContext(DeclContextKind dcTy, DeclKind dTy,
                         DeclContext *parent)
    : dcTy(dcTy), dTy(dTy), parent(parent) {
  // declContextBits.DeclKind = ty;
}

SyntaxContext &DeclContext::GetSyntaxContext() const {
  return GetParentModule()->GetSyntaxContext();
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
