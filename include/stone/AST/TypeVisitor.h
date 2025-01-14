#ifndef STONE_AST_TYPEVISITOR_H
#define STONE_AST_TYPEVISITOR_H

#include "stone/AST/Types.h"
#include "llvm/Support/ErrorHandling.h"

namespace stone {

/// TypeVisitor - This is a simple Visitor class for Swift types.
template <typename ImplTy, typename RetTy = void, typename... Args>
class TypeVisitor {
public:
  RetTy Visit(Type T, Args... args) {
    switch (T->GetKind()) {
#define TYPE(KIND, PARENT)                                                     \
  case TypeKind::KIND:                                                         \
    return static_cast<ImplTy *>(this)->Visit##KIND##Type(                     \
        static_cast<KIND##Type *>(T.GetPtr()), ::std::forward<Args>(args)...);
#include "stone/AST/TypeKind.def"
    }
    llvm_unreachable("Not reachable, all cases handled");
  }

  // Provide default implementations of abstract "Visit" implementations that
  // just chain to their base class.  This allows Visitors to just implement
  // the base behavior and handle all subclasses if they desire.  Since this is
  // a template, it will only instantiate cases that are used and thus we still
  // require full coverage of the AST nodes by the Visitor.
#define ABSTRACT_TYPE(KIND, PARENT)                                            \
  RetTy Visit##KIND##Type(KIND##Type *T, Args... args) {                       \
    return static_cast<ImplTy *>(this)->Visit##PARENT(                         \
        T, std::forward<Args>(args)...);                                       \
  }
#define TYPE(KIND, PARENT) ABSTRACT_TYPE(KIND, PARENT)
#include "stone/AST/TypeKind.def"

public:
  void VisitType(Type t) {}
};

// namespace syn {
// template <typename ImplTy, typename RetTy = void, typename... Args>
// class CanTypeVisitor {
// public:
//   RetTy Visit(CanType T, Args... args) {
//     switch (T->GetKind()) {
// #define SWEET_TYPE(KIND, PARENT) case TypeKind::KIND:
// #define TYPE(KIND, PARENT)
// #include "stone/AST/TypeKind.def"
//       llvm_unreachable("non-canonical type");

// #define SWEET_TYPE(KIND, PARENT)
// #define TYPE(KIND, PARENT) \
//   case TypeKind::KIND: \
//     return static_cast<ImplTy *>(this)->Visit##KIND##Type( \
//         cast<KIND##Type>(T), ::std::forward<Args>(args)...);
// #include "stone/AST/TypeKind.def"
//     }
//     llvm_unreachable("Not reachable, all cases handled");
//   }

//   // Provide default implementations of abstract "Visit" implementations that
//   // just chain to their base class.  This allows Visitors to just implement
//   // the base behavior and handle all subclasses if they desire.  Since this
//   is
//   // a template, it will only instantiate cases that are used and thus we
//   still
//   // require full coverage of the AST nodes by the Visitor.
// #define ABSTRACT_TYPE(KIND, PARENT) \
//   RetTy Visit##KIND##Type(Can##KIND##Type T, Args... args) { \
//     return static_cast<ImplTy *>(this)->Visit##PARENT( \
//         T, std::forward<Args>(args)...); \
//   }
// #define TYPE(KIND, PARENT) ABSTRACT_TYPE(KIND, PARENT)
// #define ABSTRACT_SWEET_TYPE(KIND, PARENT)
// #define SWEET_TYPE(KIND, PARENT)
//   // Don't allow unchecked types by default, but allow Visitors to opt-in to
//   // handling them.
// #define UNCHECKED_TYPE(KIND, PARENT) \
//   RetTy Visit##KIND##Type(Can##KIND##Type T, Args... args) { \
//     llvm_unreachable("Unchecked type"); \
//   }
// #include "stone/AST/TypeKind.def"
// };
// } // namespace syn

} // end namespace stone

#endif
