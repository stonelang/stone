#include "stone/AST/TypeLoc.h"
#include "stone/AST/TypeState.h"
#include "stone/AST/Types.h"
#include "stone/Basic/Memory.h"

using namespace stone;

static bool IsTypeKind(TypeKind kind) {
  switch (kind) {
  case TypeKind::Float:
  case TypeKind::Int:
    return true;
  default:
    return false;
  }
}

// == UnderlyingType ==//
bool UnderlyingType::IsBasic() {
  switch (GetKind()) {
  case TypeKind::Float:
  case TypeKind::Int:
    return true;
  default:
    return false;
  }
}

bool UnderlyingType::IsNominalType() {
  switch (GetKind()) {
  case TypeKind::Interface:
  case TypeKind::Struct:
    return true;
  default:
    return false;
  }
}

FunType::FunType(Type returnType, const ASTContext *astContext)
    : FunctionType(TypeKind::Fun, returnType, astContext) {}

// VoidType *VoidType::Create(const ASTContext &astContext,
// MemoryAllocationArena arena) {
//   return new (astContext) VoidType(astContext);
// }

// NullType* NullType::Create(const ASTContext& sc, MemoryAllocationArena
// arena)
// {
//   return new(sc, arena) NullType(sc);
// }
// BoolType* BoolType::Create(const ASTContext& sc, MemoryAllocationArena
// arena)
// {
//   return new(sc, arena) BoolType(sc);
// }

// == Type == //
bool Type::Walk(TypeWalker &walker) const {}

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
//   // So, look up the type from the context and apple the qualifiers to
//   it. return Type();
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

// FunType *TypeFactory::MakeFunType(Type result);

// IntegerType *TypeFactory::MakeIntegerType(NumberBitWidthKind bitWidthKind,
//                                  const ASTContext &sc) {
//   return new (sc) IntegerType(bitWidthKind, sc);
// }

// IntegerType *IntegerType::Create(NumberBitWidthKind bitWidthKind,
//                                  const ASTContext &astContext) {
//   return new (astContext) IntegerType(bitWidthKind, astContext);
// }

// FloatType *FloatType::Create(NumberBitWidthKind bitWidthKind,
//                              const ASTContext &astContext) {
//   return new (astContext) FloatType(bitWidthKind, astContext);
// }
