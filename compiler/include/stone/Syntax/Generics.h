#ifndef STONE_SYNTAX_GENERICS_H
#define STONE_SYNTAX_GENERICS_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/Identifier.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
namespace syn {}

} // namespace stone

#endif