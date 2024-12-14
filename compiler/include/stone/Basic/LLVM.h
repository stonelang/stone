#ifndef STONE_BASIC_LLVM_H
#define STONE_BASIC_LLVM_H

// Do not proliferate #includes here, require clients to #include their
// dependencies.
// Casting.h has complex templates that cannot be easily forward declared.
#include "llvm/Support/Casting.h"

#include <optional>

namespace llvm {
// ADT's.
class StringRef;
class StringLiteral;
class Twine;
class VersionTuple;

// Other common classes.
class raw_ostream;
class raw_ostream;
class raw_pwrite_stream;
class APInt;
class APFloat;

template <typename T> class ArrayRef;
template <typename T> class MutableArrayRef;
template <unsigned InternalLen> class SmallString;
template <typename T, unsigned N> class SmallVector;
template <typename T> class SmallVectorImpl;
template <typename T> class TinyPtrVector;

template <typename T> struct SaveAndRestore;

// Reference counting.
template <typename T> class IntrusiveRefCntPtr;
template <typename T> struct IntrusiveRefCntPtrInfo;
template <class Derived> class RefCountedBase;

template <typename... PTs> class PointerUnion;
template <typename IteratorT> class iterator_range;
class SmallBitVector;

// TODO: DenseMap, ...
} // namespace llvm

namespace stone {
// Casting operators.
using llvm::cast;
using llvm::cast_or_null;
using llvm::dyn_cast;
using llvm::dyn_cast_or_null;
using llvm::isa;
using llvm::isa_and_nonnull;

// ADT's.
using llvm::ArrayRef;
using llvm::MutableArrayRef;
using llvm::SaveAndRestore;
using llvm::SmallString;
using llvm::SmallVector;
using llvm::SmallVectorImpl;
using llvm::StringLiteral;
using llvm::StringRef;
using llvm::TinyPtrVector;
using llvm::Twine;
using llvm::VersionTuple;

using std::nullopt;

// Reference counting.
using llvm::IntrusiveRefCntPtr;
using llvm::IntrusiveRefCntPtrInfo;
using llvm::RefCountedBase;

using llvm::raw_ostream;
using llvm::raw_pwrite_stream;

// Other common classes.
using llvm::APFloat;
using llvm::APInt;

} // namespace stone

#endif
