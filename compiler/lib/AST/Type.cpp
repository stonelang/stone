#include "stone/AST/TypeLoc.h"
#include "stone/AST/TypeQualifier.h"
#include "stone/AST/Types.h"

using namespace stone;

// == TypeBase ==//
// bool TypeBase::IsBasic() {
//   switch (GetKind()) {
//   case TypeKind::Float:
//   case TypeKind::Integer:
//     return true;
//   default:
//     return false;
//   }
// }

// TypeKind Type::GetKind() const {
//   assert(GetPtr());
//   return GetPtr()->GetKind();
// }

// bool TypeBase::IsNominalType() {
//   switch (GetKind()) {
//   case TypeKind::Interface:
//   case TypeKind::Struct:
//     return true;
//   default:
//     return false;
//   }
// }

VoidType *VoidType::Create(const ASTContext &astContext, AllocationArena arena,
                           TypeQualifiers qualifiers) {
  return new (astContext, arena) VoidType(astContext, qualifiers);
}

NullType *NullType::Create(const ASTContext &astContext,
                           AllocationArena arena) {
  return new (astContext, arena) NullType(astContext);
}

BoolType *BoolType::Create(const ASTContext &astContext, AllocationArena arena,
                           TypeQualifiers qualifiers) {
  return new (astContext, arena) BoolType(astContext, qualifiers);
}

IntegerType *IntegerType::Create(NumberBitWidthKind bitWidthKind,
                                 AllocationArena arena,
                                 const ASTContext &astContext,
                                 TypeQualifiers qualifiers) {

  return new (astContext, arena)
      IntegerType(bitWidthKind, astContext, qualifiers);
}

FloatType *FloatType::Create(NumberBitWidthKind bitWidthKind,
                             const ASTContext &astContext,
                             AllocationArena arena, TypeQualifiers qualifiers) {
  return new (astContext, arena)
      FloatType(bitWidthKind, astContext, qualifiers);
}

FloatType *UIntegerType::Create(NumberBitWidthKind bitWidthKind,
                                const ASTContext &astContext,
                                AllocationArena arena,
                                TypeQualifiers qualifiers) {
  return new (astContext, arena)
      UIntegerType(bitWidthKind, astContext, qualifiers);
}

// == Type == //
// bool Type::Walk(TypeWalker &walker) const {}

// bool Type::IsBuiltinType() const {}

// bool Type::IsFunType() const {}

// bool Type::IsStructType() const {}

// bool Type::IsPointerType() const {}

// bool Type::IsReferenceType() const {}

// == TypeQualifierCollector == //

/// Collect any qualifiers on the given type and return an
/// unqualified type.  The qualifiers are assumed to be consistent
/// with those already in the type.
// const Type *TypeQualifierCollector::StripQualsFromType(Type type) {
//   // TODO:
//   //  AddFastQualifiers(type.GetLocalFastQualifiers());
//   //  if (!type.HasLocalNonFastQualifiers()){
//   //    return type.GetTypePtrUnsafe();
//   //  }
//   return nullptr;
// }

// Type TypeQualifierCollector::ApplyQualsToType(const ASTContext &sc,
//                                                   Type qt) const {
//   // You can do this because the type was saved in the ASTContext
//   // So, look up the type from the context and apple the qualifiers to it.
//   return Type();
// }

// // THINK about this
// /// Apply the collected qualifiers to the given type.
// Type TypeQualifierCollector::ApplyQualsToType(const ASTContext
// &Context,
//                                        const Type *ty) const {}

// == TypeLoc == //
bool TypeLoc::IsError() const { return true; }

SrcLoc TypeLoc::GetLoc() const { return SrcLoc(); }

SrcRange TypeLoc::GetSrcRange() const { return SrcRange(); }

void TypeLoc::SetType(Type ty) {}

FunType::FunType(Type result, const ASTContext *astContext,
                 TypeQualifiers qualifiers)
    : FunctionType(TypeKind::Fun, result, astContext, qualifiers) {}

// FunType *TypeFactory::MakeFunType(Type result);

// IntegerType *TypeFactory::MakeIntegerType(NumberBitWidthKind bitWidthKind,
//                                  const ASTContext &sc) {
//   return new (sc) IntegerType(bitWidthKind, sc);
// }
