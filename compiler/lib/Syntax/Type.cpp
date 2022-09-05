#include "stone/Syntax/TypeLoc.h"
#include "stone/Syntax/Type.h"

using namespace stone::syn;

bool Type::Walk(TypeWalker &walker) const {}

// SrcLoc TypeLoc::GetLoc() const {

// }
// SrcRange TypeLoc::GetSrcRange() const {

// }
// void TypeLoc::SetType(Type ty) {

// }

// QualType TypeQualifierCollector::Apply(const SyntaxContext &sc, QualType qt)
// {}

// // THINK about this
// /// Apply the collected qualifiers to the given type.
// QualType TypeQualifierCollector::Apply(const SyntaxContext &Context,
//                                        const Type *ty) const {}