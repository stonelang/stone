#include "stone/AST/Type.h"
#include "stone/AST/TypeState.h"
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

// == Type ==//
bool Type::IsBuiltinType() const {
  switch (GetKind()) {
  case TypeKind::Float:
  case TypeKind::Int:
    return true;
  default:
    return false;
  }
}

bool Type::IsNominalType() {
  switch (GetKind()) {
  case TypeKind::Interface:
  case TypeKind::Struct:
    return true;
  default:
    return false;
  }
}

FunType::FunType(TypeState *returnType)
    : FunctionType(TypeKind::Fun, returnType) {}

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

BitWidth NumberType::GetBitWidth() const {
  switch (GetKind()) {
  case TypeKind::Int:
  case TypeKind::UInt:
  case TypeKind::Float:
    return BitWidth::Size;
  case TypeKind::Int8:
    return BitWidth::Size8;
  case TypeKind::Int16:
    return BitWidth::Size16;
  case TypeKind::Int32:
    return BitWidth::Size32;
  case TypeKind::Int64:
    return BitWidth::Size64;
  case TypeKind::Int128:
    return BitWidth::Size128;
  case TypeKind::UInt8:
    return BitWidth::Size8;
  case TypeKind::UInt16:
    return BitWidth::Size16;
  case TypeKind::UInt32:
    return BitWidth::Size32;
  case TypeKind::UInt64:
    return BitWidth::Size64;
  case TypeKind::UInt128:
    return BitWidth::Size128;
  case TypeKind::Float16:
    return BitWidth::Size16;
  case TypeKind::Float32:
    return BitWidth::Size32;
  case TypeKind::Float64:
    return BitWidth::Size64;
  case TypeKind::Float128:
    return BitWidth::Size128;
  case TypeKind::Complex32:
    return BitWidth::Size32;
  case TypeKind::Imaginary32:
    return BitWidth::Size32;
  case TypeKind::Imaginary64:
    return BitWidth::Size64;
  }
  llvm_unreachable("Unknown number type");
}

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

SrcLoc TypeState::GetLoc() const { return SrcLoc(); }
SrcRange TypeState::GetSrcRange() const { return SrcRange(); }

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

// == Type == //
bool TypeState::Walk(TypeWalker &walker) const {}
