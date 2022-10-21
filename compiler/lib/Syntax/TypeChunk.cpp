#include "stone/Syntax/TypeChunk.h"
#include "stone/Syntax/SyntaxContext.h"

using namespace stone::syn;

PointerTypeChunk PointerTypeChunk::Create(SrcLoc loc) {
  PointerTypeChunk chunk(loc);
  return chunk;
}

ReferenceTypeChunk ReferenceTypeChunk::Create(SrcLoc loc) {
  ReferenceTypeChunk chunk(loc);
  return chunk;
}

ValueTypeChunk ValueTypeChunk::Create() {
  ValueTypeChunk chunk;
  return chunk;
}

ArrayTypeChunk ArrayTypeChunk::Create(SrcLoc loc) {
  ArrayTypeChunk chunk(loc);
  return chunk;
}

void TypeChunkCollector::AddValue() { AddTypeChunk(ValueTypeChunk::Create()); }
void TypeChunkCollector::AddPointer(SrcLoc inputLoc) {
  AddTypeChunk(PointerTypeChunk::Create(inputLoc));
}
void TypeChunkCollector::AddReference(SrcLoc inputLoc) {
  AddTypeChunk(ReferenceTypeChunk::Create(inputLoc));
}
void TypeChunkCollector::AddArray(SrcLoc inputLoc) {
  AddTypeChunk(ArrayTypeChunk::Create(inputLoc));
}

void TypeChunkCollector::AddParen(SrcLoc inputLoc) {}
void TypeChunkCollector::AddPipe(SrcLoc inputLoc) {}
void TypeChunkCollector::Apply() {}

TypeChunkList::TypeChunkList(llvm::ArrayRef<TypeChunk> chunks) {
  std::uninitialized_copy(chunks.begin(), chunks.end(),
                          getTrailingObjects<TypeChunk>());
}

TypeChunkList *TypeChunkList::Create(llvm::ArrayRef<TypeChunk> chunks,
                                     SyntaxContext &sc) {

  unsigned sizeToAlloc =
      TypeChunkList::totalSizeToAlloc<TypeChunk>(chunks.size());
  void *memPtr = sc.Allocate(sizeToAlloc, alignof(TypeChunkList));
  return new (memPtr) TypeChunkList(llvm::MutableArrayRef<TypeChunk>());
}
