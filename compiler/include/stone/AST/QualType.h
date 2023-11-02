#ifndef STONE_AST_QUALTYPE_H
#define STONE_AST_QUALTYPE_H

#include "stone/AST/TypeKind.h"
#include "stone/AST/TypeQualifier.h"

namespace stone {
namespace ast {

// class Type;
// class QualType {
//   friend class TypeCollector;

//   Type *typePtr = nullptr;

//   TypeQualifier constTypeQual{TypeQualifierKind::Const};
//   TypeQualifier restrictTypeQual{TypeQualifierKind::Restrict};
//   TypeQualifier volatileTypeQual{TypeQualifierKind::Volatile};
//   TypeQualifier pureTypeQual{TypeQualifierKind::Pure};
//   TypeQualifier immutableTypeQual{TypeQualifierKind::Immutable};
//   TypeQualifier mutableTypeQual{TypeQualifierKind::Mutable};

// public:
//   QualType(Type *typePtr = nullptr) : typePtr(typePtr) {}

//   // QualType(Type *typePtr, TypeChunkList *thunks = nullptr)
//   //     : typePtr(typePtr), thunks(thunks) {}

// public:
//   bool IsNull() const { return typePtr == nullptr; }
//   Type *GetPtr() const { return typePtr; }

//   TypeKind GetKind() const;

//   Type *operator->() const {
//     assert(typePtr && "Cannot dereference a null Type!");
//     return typePtr;
//   }
//   explicit operator bool() const { return typePtr != nullptr; }

// public:
//   /// Walk this Type.
//   ///
//   /// Returns true if the walk was aborted.
//   // bool Walk(TypeWalker &walker) const;
//   // bool Walk(TypeWalker &&walker) const { return Walk(walker); }

// public:
//   /// Look through the given Type and its children to find a Type
//   /// for which the given predicate returns true.
//   ///
//   /// \param pred A predicate function object. It should return true if the
//   give
//   /// Type node satisfies the criteria.
//   ///
//   /// \returns true if the predicate returns true for the given Type or
//   /// any of its children.
//   bool FindIf(llvm::function_ref<bool(QualType)> pred) const;

//   /// Transform the given Type by applying the user-provided function to
//   /// each Type.
//   ///
//   /// This routine applies the given function to transform one Type into
//   /// another. If the function leaves the Type unchanged, recurse into the
//   /// child Type nodes and transform those. If any child Type node
//   /// changes, the parent Type node will be rebuilt.
//   ///
//   /// If at any time the function returns a null Type, the null will be
//   /// propagated out.
//   ///
//   /// \param fn A function object with the signature \c Type(Type),
//   /// which accepts a Type and returns either a transformed Type or
//   /// a null Type.
//   ///
//   /// \returns the result of transforming the Type.
//   QualType Transform(llvm::function_ref<QualType(QualType)> fn) const;

//   /// Transform the given Type by applying the user-provided function to
//   /// each Type.
//   ///
//   /// This routine applies the given function to transform one Type into
//   /// another. If the function leaves the Type unchanged, recurse into the
//   /// child Type nodes and transform those. If any child Type node
//   /// changes, the parent Type node will be rebuilt.
//   ///
//   /// If at any time the function returns a null Type, the null will be
//   /// propagated out.
//   ///
//   /// If the function returns \c None, the transform operation will
//   ///
//   /// \param fn A function object with the signature
//   /// \c Optional<Type>(Type *), which accepts a Type
//   /// pointer and returns a transformed Type, a null Type (which
//   /// will propagate the null Type to the outermost \c transform() call),
//   /// or None (to indicate that the transform operation should recursively
//   /// transform the subTypes). The function object should use \c dyn_cast
//   /// rather \c getAs, because the transform itself handles desugaring.
//   ///
//   /// \returns the result of transforming the Type.
//   QualType
//   TransformRec(llvm::function_ref<llvm::Optional<QualType>(Type *)> fn)
//   const;

//   /// Look through the given Type and its children and apply fn to them.
//   void Visit(llvm::function_ref<void(QualType)> fn) const {
//     FindIf([&fn](QualType t) -> bool {
//       fn(t);
//       return false;
//     });
//   }

//   /// Replace references to substitutable Types with new, concrete
//   /// Types and return the substituted result.
//   ///
//   /// \param substitutions The mapping from substitutable Types to their
//   /// replacements and conformances.
//   ///
//   /// \param options Options that affect the substitutions.
//   ///
//   /// \returns the substituted Type, or a null Type if an error
//   /// occurred.
//   // Type Substitute(SubstitutionMap substitutions,
//   //                 SubstitutionOptions options = None) const;

//   // /// Replace references to substitutable Types with new, concrete
//   // Types and
//   /// return the substituted result.
//   ///
//   /// \param substitutions A function mapping from substitutable Types to
//   /// their replacements.
//   ///
//   /// \param conformances A function for looking up conformances.
//   ///
//   /// \param options Options that affect the substitutions.
//   ///
//   /// \returns the substituted Type, or a null Type if an error
//   /// occurred.
//   // Type Substitute(TypeSubstitutionFn substitutions,
//   //                 LookupConformanceFn conformances,
//   //                 SubstOptions options = None) const;

// public:
//   bool IsBuiltinType() const;
//   bool IsFunType() const;
//   bool IsStructType() const;
//   bool IsPointerType() const;
//   bool IsReferenceType() const;

// public:
// public:
//   TypeQualifier &GetConst() { return constTypeQual; }
//   TypeQualifier &GetRestrict() { return restrictTypeQual; }
//   TypeQualifier &GetVolatile() { return volatileTypeQual; }
//   TypeQualifier &GetPure() { return pureTypeQual; }
//   TypeQualifier &GetImmutable() { return immutableTypeQual; }
//   TypeQualifier &GetMutable() { return mutableTypeQual; }

// public:
//   bool HasAny() {
//     return GetConst().IsValid() || GetRestrict().IsValid() ||
//            GetVolatile().IsValid() || GetPure().IsValid() ||
//            GetImmutable().IsValid() || GetMutable().IsValid();
//   }
//   void ClearAll() {
//     GetConst().Clear();
//     GetRestrict().Clear();
//     GetVolatile().Clear();
//     GetPure().Clear();
//     GetImmutable().Clear();
//     GetMutable().Clear();
//   }

//   // private:
//   //   // Direct comparison is disabled for types, because they may not be
//   //   canonical. void operator==(QualType T) const = delete; void
//   //   operator!=(QualType T) const = delete;
// };
} // namespace ast
} // namespace stone

#endif