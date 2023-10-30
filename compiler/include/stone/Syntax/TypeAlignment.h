#ifndef STONE_SYNTAX_TYPEALIGNMENT_H
#define STONE_SYNTAX_TYPEALIGNMENT_H

#include <cstddef>

namespace stone {

namespace syn {
class Decl;
class Expr;
class Stmt;
class ValueDecl;
class Type;
class TypeDecl;
class TypeBase;
class InterfaceDecl;
class SyntaxContext;
class DeclContext;
class ModuleFile;
class Attribute;

} // namespace syn

constexpr size_t AttributeAlignInBits = 3;
constexpr size_t DeclAlignInBits = 3;
constexpr size_t ExprAlignInBits = 3;
constexpr size_t StmtAlignInBits = 3;
constexpr size_t TypeAlignInBits = 3;
constexpr size_t SyntaxContextAlignInBits = 2;
constexpr size_t DeclContextAlignInBits = 3;

} // namespace stone

namespace llvm {
/// Helper class for declaring the expected alignment of a pointer.
/// TODO: LLVM should provide this.
template <class T, size_t AlignInBits> struct MoreAlignedPointerTraits {
  enum { NumLowBitsAvailable = AlignInBits };
  static inline void *getAsVoidPointer(T *ptr) { return ptr; }
  static inline T *getFromVoidPointer(void *ptr) {
    return static_cast<T *>(ptr);
  }
};

template <class T> struct PointerLikeTypeTraits;
} // namespace llvm

/// Declare the expected alignment of pointers to the given type.
/// This macro should be invoked from a top-level file context.
#define LLVM_DECLARE_TYPE_ALIGNMENT(CLASS, ALIGNMENT)                          \
  namespace llvm {                                                             \
  template <>                                                                  \
  struct PointerLikeTypeTraits<CLASS *>                                        \
      : public MoreAlignedPointerTraits<CLASS, ALIGNMENT> {};                  \
  }

LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::Decl, stone::DeclAlignInBits)
LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::InterfaceDecl, stone::DeclAlignInBits)
LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::TypeDecl, stone::DeclAlignInBits)
LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::ValueDecl, stone::DeclAlignInBits)

LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::Stmt, stone::StmtAlignInBits)
LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::Expr, stone::ExprAlignInBits)
LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::SyntaxContext,
                            stone::SyntaxContextAlignInBits)

LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::DeclContext,
                            stone::DeclContextAlignInBits)

LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::ModuleFile,
                            stone::SyntaxContextAlignInBits)

LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::TypeBase, stone::TypeAlignInBits)
LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::Type, stone::TypeAlignInBits)
LLVM_DECLARE_TYPE_ALIGNMENT(stone::syn::Attribute, stone::AttributeAlignInBits)

static_assert(alignof(void *) >= 2, "pointer alignment is too small");

#endif
