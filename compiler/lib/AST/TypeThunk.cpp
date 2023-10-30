#include "stone/AST/TypeThunk.h"
#include "stone/AST/ASTContext.h"

using namespace stone::ast;

PointerTypeThunk PointerTypeThunk::Create(SrcLoc loc) {
  PointerTypeThunk chunk(loc);
  return chunk;
}

ReferenceTypeThunk ReferenceTypeThunk::Create(SrcLoc loc) {
  ReferenceTypeThunk chunk(loc);
  return chunk;
}

ValueTypeThunk ValueTypeThunk::Create() {
  ValueTypeThunk chunk;
  return chunk;
}

ArrayTypeThunk ArrayTypeThunk::Create(SrcLoc loc) {
  ArrayTypeThunk chunk(loc);
  return chunk;
}

void TypeThunkCollector::AddValue() { AddTypeThunk(ValueTypeThunk::Create()); }

void TypeThunkCollector::AddPointer(SrcLoc inputLoc) {
  AddTypeThunk(PointerTypeThunk::Create(inputLoc));
}
void TypeThunkCollector::AddReference(SrcLoc inputLoc) {
  AddTypeThunk(ReferenceTypeThunk::Create(inputLoc));
}
void TypeThunkCollector::AddArray(SrcLoc inputLoc) {
  AddTypeThunk(ArrayTypeThunk::Create(inputLoc));
}

void TypeThunkCollector::AddParen(SrcLoc inputLoc) {}

void TypeThunkCollector::AddPipe(SrcLoc inputLoc) {}

void TypeThunkCollector::Apply() {}

TypeThunkList::TypeThunkList(llvm::ArrayRef<TypeThunk> chunks) {
  std::uninitialized_copy(chunks.begin(), chunks.end(),
                          getTrailingObjects<TypeThunk>());
}

TypeThunkList *TypeThunkList::Create(llvm::ArrayRef<TypeThunk> chunks,
                                     ASTContext &sc) {

  unsigned sizeToAlloc =
      TypeThunkList::totalSizeToAlloc<TypeThunk>(chunks.size());
  void *memPtr = sc.Allocate(sizeToAlloc, alignof(TypeThunkList));
  return new (memPtr) TypeThunkList(llvm::MutableArrayRef<TypeThunk>());
}
