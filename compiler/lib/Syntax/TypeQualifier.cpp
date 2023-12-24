#include "stone/Syntax/TypeQualifier.h"
#include "stone/Syntax/ASTContext.h"

using namespace stone;

// ConstTypeQualifier ConstTypeQualifier::Create(SrcLoc loc) {
//   ConstTypeQualifier qualifier(loc);
//   return qualifier;
// }

// RestrictTypeQualifier RestrictTypeQualifier::Create(SrcLoc loc) {
//   RestrictTypeQualifier qualifier(loc);
//   return qualifier;
// }

// VolatileTypeQualifier VolatileTypeQualifier::Create(SrcLoc loc) {
//   VolatileTypeQualifier qualifier(loc);
//   return qualifier;
// }

// UnalignedTypeQualifier UnalignedTypeQualifier::Create(SrcLoc loc) {
//   UnalignedTypeQualifier qualifier(loc);
//   return qualifier;
// }

// ImmutableTypeQualifier ImmutableTypeQualifier::Create(SrcLoc loc) {
//   ImmutableTypeQualifier qualifier(loc);
//   return qualifier;
// }

// MutableTypeQualifier MutableTypeQualifier::Create(SrcLoc loc) {
//   MutableTypeQualifier qualifier(loc);
//   return qualifier;
// }

// TypeQualifierList::TypeQualifierList(llvm::ArrayRef<TypeQualifier>
// qualifiers) {
//   std::uninitialized_copy(qualifiers.begin(), qualifiers.end(),
//                           getTrailingObjects<TypeQualifier>());
// }

// TypeQualifierList *
// TypeQualifierList::Create(llvm::ArrayRef<TypeQualifier> qualifiers,
//                           ASTContext &sc) {

//   unsigned sizeToAlloc =
//       TypeQualifierList::totalSizeToAlloc<TypeQualifier>(qualifiers.size());
//   void *memPtr = sc.Allocate(sizeToAlloc, alignof(TypeQualifierList));
//   return new (memPtr)
//   TypeQualifierList(llvm::MutableArrayRef<TypeQualifier>());
// }

// TypeQualifierCollector::TypeQualifierCollector() {}
