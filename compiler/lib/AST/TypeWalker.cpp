#include "stone/AST/TypeWalker.h"
#include "stone/AST/TypeVisitor.h"

using namespace stone;

// namespace {
// /// This class implements a simple type recursive traverser which queries a
// /// user-provided walker class on every node in a type.
// class TypeTraversal : public TypeVisitor<TypeTraversal, bool> {
//   // using Base = TypeVisitor;
//   // friend Base;
//   // TypeWalker &Walker;

//   // bool visitErrorType(ErrorType *ty) { return false; }
//   // bool visitUnresolvedType(UnresolvedType *ty) { return false; }
//   // bool visitPlaceholderType(PlaceholderType *ty) { return false; }
//   // bool visitBuiltinType(BuiltinType *ty) { return false; }
//   bool VisitAliasType(AliasType *ty) {}

//   bool VisitNominalType(NominalType *ty) {}
// }
