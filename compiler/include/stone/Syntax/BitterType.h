#ifndef STONE_SYNTAX_CANTYPE_H
#define STONE_SYNTAX_CANTYPE_H

#include "stone/Basic/STDTypeAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Foreign/Foreign.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/type_traits.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

#include <string>

namespace stone {
namespace syn {

enum class BitterTypeKind : UInt8 {
    None = 0,

};

class alignas(1 << TypeAlignInBits) BitterType
    : public SyntaxAllocation<std::aligned_storage<8, 8>::type> {

  friend class SyntaxContext;
  BitterType(const BitterType &) = delete;
  void operator=(const BitterType &) = delete;

    BitterTypeKind kind; 
public:
    BitterTypeKind GetKind() { return kind; }
};

} // namespace syn

} // namespace stone

#endif
