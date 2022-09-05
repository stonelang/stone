#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeLoc.h"

using namespace stone::syn;

bool Type::Walk(TypeWalker &walker) const {}

// SrcLoc TypeLoc::GetLoc() const {

// }
// SrcRange TypeLoc::GetSrcRange() const {

// }
// void TypeLoc::SetType(Type ty) {

// }

QualType TypeQualifierCollector::ApplyToType(const SyntaxContext &sc, QualType qt) const {

  // You can do this because the type was saved in the SyntaxContext
  // So, look up the type from the context and apple the qualifiers to it.
  return QualType();
}

// // THINK about this
// /// Apply the collected qualifiers to the given type.
// QualType TypeQualifierCollector::ApplyQualifiers(const SyntaxContext
// &Context,
//                                        const Type *ty) const {}