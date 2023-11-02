#ifndef STONE_AST_CANTYPE_H
#define STONE_AST_CANTYPE_H

#include "stone/AST/QualType.h"

namespace stone {
namespace ast {

// class Type;
// class CanType final : public QualType {
// public:
//   /// Constructs a NULL canonical type.
//   CanType() = default;

// public:
//   explicit CanType(Type *typePtr) : QualType(typePtr) {

//     //   assert(IsCanTypeOrNull() &&
//     //          "Forming a CanType out of a non-canonical type!");
//   }

//   // explicit CanType(Type ty) : Type(ty) {
//   //   assert(IsCanTypeOrNull() &&
//   //          "Forming a CanType out of a non-canonical type!");
//   // }

//   // explicit CanType(Type *ty, TypeQualifierList *quals,
//   //                  TypeChunkList *thunks)
//   //     : CanType(ty, quals, thunks) {
//   //   assert(IsCanTypeOrNull() &&
//   //          "Forming a CanType out of a non-canonical type!");
//   // }
//   // explicit CanType(QualType ty) : QualType(ty) {
//   //   assert(IsCanTypeOrNull() &&
//   //          "Forming a CanType out of a non-canonical type!");
//   // }
// private:
//   // bool IsCanTypeOrNull() const { return true; }

//   // public:
//   //   void Visit(llvm::function_ref<void(CanType)> fn) const {
//   //     FindIf([&fn](QualType t) -> bool {
//   //       fn(CanType(t));
//   //       return false;
//   //     });
//   //   }
//   //   bool FindIf(llvm::function_ref<bool(CanType)> fn) const {
//   //     return QualType::FindIf([&fn](QualType t) { return fn(CanType(t));
//   });
//   //   }

// public:
//   // Direct comparison is allowed for CanTypes - they are known canonical.
//   // bool operator==(CanType T) const { return GetPtr() == T.GetPtr(); }
//   // bool operator!=(CanType T) const { return !operator==(T); }
//   // bool operator<(CanType T) const { return GetPtr() < T.GetPtr(); }
// };
} // namespace ast
} // namespace stone

#endif
