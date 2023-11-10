//===--- TypeVisitor.h - Type Visitor ---------------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
// This file defines the TypeVisitor class.
//
//===----------------------------------------------------------------------===//

#ifndef STONE_ASTTYPEVISITOR_H
#define STONE_ASTTYPEVISITOR_H

#include "stone/AST/Types.h"
#include "llvm/Support/ErrorHandling.h"

namespace stone {

// template<typename ImplClass, typename RetTy = void, typename... Args>
// class CanTypeVisitor {
// public:
//   RetTy visit(CanType T, Args... args) {
//     switch (T->getKind()) {
// #define SUGARED_TYPE(CLASS, PARENT) \
//     case TypeKind::CLASS:
// #define TYPE(CLASS, PARENT)
// #include "swift/AST/TypeNodes.def"
//       llvm_unreachable("non-canonical type");

// #define SUGARED_TYPE(CLASS, PARENT)
// #define TYPE(CLASS, PARENT)                                  \
//     case TypeKind::CLASS:                                    \
//       return static_cast<ImplClass*>(this)                   \
//         ->visit##CLASS##Type(cast<CLASS##Type>(T),           \
//                              ::std::forward<Args>(args)...);
// #include "stone/AST/TypeKind.def"
//     }
//     llvm_unreachable("Not reachable, all cases handled");
//   }

//   // Provide default implementations of abstract "visit" implementations that
//   // just chain to their base class.  This allows visitors to just implement
//   // the base behavior and handle all subclasses if they desire.  Since this
//   is
//   // a template, it will only instantiate cases that are used and thus we
//   still
//   // require full coverage of the AST nodes by the visitor.
// #define ABSTRACT_TYPE(CLASS, PARENT)                           \
//   RetTy visit##CLASS##Type(Can##CLASS##Type T, Args... args) { \
//      return static_cast<ImplClass*>(this)                      \
//               ->visit##PARENT(T, std::forward<Args>(args)...); \
//   }
// #define TYPE(CLASS, PARENT) ABSTRACT_TYPE(CLASS, PARENT)
// #define ABSTRACT_SUGARED_TYPE(CLASS, PARENT)
// #define SUGARED_TYPE(CLASS, PARENT)
//   // Don't allow unchecked types by default, but allow visitors to opt-in to
//   // handling them.
// #define UNCHECKED_TYPE(CLASS, PARENT)                          \
//   RetTy visit##CLASS##Type(Can##CLASS##Type T, Args... args) { \
//      llvm_unreachable("unchecked type");                       \
//   }
// #include "stone/AST/TypeKind.def"
// };

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

// namespace ast {
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
// } // namespace ast

} // end namespace stone

#endif
