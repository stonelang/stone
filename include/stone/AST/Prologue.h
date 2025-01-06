#ifndef STONE_AST_PROLOGUE_H
#define STONE_AST_PROLOGUE_H

#include "stone/AST/Identifier.h"
#include "stone/AST/PrologueKind.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {

class alignas(1 << PrologueAlignInBits) Prologue
    : public ASTAllocation<Prologue> {};

class DeclPrologue : public Prologue {

  
};

} // namespace stone

#endif