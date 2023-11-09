#include "stone/AST/TypeSlab.h"
#include "stone/AST/ASTContext.h"

using namespace stone;

PointerTypeSlab PointerTypeSlab::Create(SrcLoc loc) {
  PointerTypeSlab chunk(loc);
  return chunk;
}

ReferenceTypeSlab ReferenceTypeSlab::Create(SrcLoc loc) {
  ReferenceTypeSlab chunk(loc);
  return chunk;
}

ValueTypeSlab ValueTypeSlab::Create() {
  ValueTypeSlab chunk;
  return chunk;
}

ArrayTypeSlab ArrayTypeSlab::Create(SrcLoc loc) {
  ArrayTypeSlab chunk(loc);
  return chunk;
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

TypeSlabList::TypeSlabList(llvm::ArrayRef<TypeSlab> chunks) {
  std::uninitialized_copy(chunks.begin(), chunks.end(),
                          getTrailingObjects<TypeSlab>());
}

TypeSlabList *TypeSlabList::Create(llvm::ArrayRef<TypeSlab> chunks,
                                     ASTContext &sc) {

  unsigned sizeToAlloc =
      TypeSlabList::totalSizeToAlloc<TypeSlab>(chunks.size());
  void *memPtr = sc.Allocate(sizeToAlloc, alignof(TypeSlabList));
  return new (memPtr) TypeSlabList(llvm::MutableArrayRef<TypeSlab>());
}
