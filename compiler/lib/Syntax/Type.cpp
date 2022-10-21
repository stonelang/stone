#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeLoc.h"

#include "stone/Syntax/TypeQualifier.h"

using namespace stone;
using namespace stone::syn;

// == TypeBase ==//
bool TypeBase::IsBasic() {
  switch (GetKind()) {
  case TypeKind::Float:
  case TypeKind::Integer:
    return true;
  default:
    return false;
  }
}

bool TypeBase::IsNominalType() {
  switch (GetKind()) {
  case TypeKind::Interface:
  case TypeKind::Struct:
    return true;
  default:
    return false;
  }
}

// == Type == //
bool Type::Walk(TypeWalker &walker) const {}

// == TypeQualifierCollector == //

/// Collect any qualifiers on the given type and return an
/// unqualified type.  The qualifiers are assumed to be consistent
/// with those already in the type.
// const Type *TypeQualifierCollector::StripQualsFromType(QualType type) {
//   // TODO:
//   //  AddFastQualifiers(type.GetLocalFastQualifiers());
//   //  if (!type.HasLocalNonFastQualifiers()){
//   //    return type.GetTypePtrUnsafe();
//   //  }
//   return nullptr;
// }

// QualType TypeQualifierCollector::ApplyQualsToType(const SyntaxContext &sc,
//                                                   QualType qt) const {
//   // You can do this because the type was saved in the SyntaxContext
//   // So, look up the type from the context and apple the qualifiers to it.
//   return QualType();
// }

// // THINK about this
// /// Apply the collected qualifiers to the given type.
// QualType TypeQualifierCollector::ApplyQualsToType(const SyntaxContext
// &Context,
//                                        const Type *ty) const {}

// == TypeLoc == //
bool TypeLoc::IsError() const { return true; }

SrcLoc TypeLoc::GetLoc() const { return SrcLoc(); }

SrcRange TypeLoc::GetSrcRange() const { return SrcRange(); }

void TypeLoc::SetType(Type ty) {}

FunctionType::FunctionType(TypeQualifierList *qualifiers, Type result,
                           TypeChunkList *chunks, const SyntaxContext *sc)
    : AbstractFunctionType(TypeKind::Function, qualifiers, result, chunks, sc) {
}

IntegerType *IntegerType::Create(NumberBitWidthKind bitWidthKind,
                                 TypeQualifierList *qualifiers,
                                 TypeChunkList *chunks,
                                 const SyntaxContext &sc) {
  return new (sc) IntegerType(bitWidthKind, qualifiers, chunks, sc);
}
