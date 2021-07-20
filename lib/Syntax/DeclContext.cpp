#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Syntax.h"

#include "stone/Core/LLVM.h"
#include "stone/Core/LangOptions.h"
#include "stone/Core/SrcLoc.h"

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

DeclContext::DeclContext(DeclContext::Type dcTy, Decl::Type dTy,
                         DeclContext *parent)
    : dcTy(dcTy), dTy(dTy), parent(parent) {
  // declContextBits.DeclType = ty;
}