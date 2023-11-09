#include "stone/AST/TypeSlab.h"
#include "stone/AST/ASTContext.h"

using namespace stone;

PointerTypeSlab PointerTypeSlab::Create(SrcLoc loc) {
  PointerTypeSlab slab(loc);
  return slab;
}

ReferenceTypeSlab ReferenceTypeSlab::Create(SrcLoc loc) {
  ReferenceTypeSlab slab(loc);
  return slab;
}

ValueTypeSlab ValueTypeSlab::Create() {
  ValueTypeSlab slab;
  return slab;
}

ArrayTypeSlab ArrayTypeSlab::Create(SrcLoc loc) {
  ArrayTypeSlab slab(loc);
  return slab;
}

void TypeSlabCollector::AddValue() { AddTypeSlab(ValueTypeSlab::Create()); }

void TypeSlabCollector::AddPointer(SrcLoc inputLoc) {
  AddTypeSlab(PointerTypeSlab::Create(inputLoc));
}
void TypeSlabCollector::AddReference(SrcLoc inputLoc) {
  AddTypeSlab(ReferenceTypeSlab::Create(inputLoc));
}
void TypeSlabCollector::AddArray(SrcLoc inputLoc) {
  AddTypeSlab(ArrayTypeSlab::Create(inputLoc));
}

void TypeSlabCollector::AddParen(SrcLoc inputLoc) {}

void TypeSlabCollector::AddPipe(SrcLoc inputLoc) {}

void TypeSlabCollector::Apply() {}

TypeSlabList::TypeSlabList(llvm::ArrayRef<TypeSlab> slabs) {
  std::uninitialized_copy(slabs.begin(), slabs.end(),
                          getTrailingObjects<TypeSlab>());
}

TypeSlabList *TypeSlabList::Create(llvm::ArrayRef<TypeSlab> slabs,
                                     ASTContext &sc) {

  unsigned sizeToAlloc =
      TypeSlabList::totalSizeToAlloc<TypeSlab>(slabs.size());
  void *memPtr = sc.Allocate(sizeToAlloc, alignof(TypeSlabList));
  return new (memPtr) TypeSlabList(llvm::MutableArrayRef<TypeSlab>());
}
