#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeChunk.h"
#include "stone/Syntax/TypeOperator.h"
#include "stone/Syntax/TypeQualifier.h"

using namespace stone;

NewTypeOperator NewTypeOperator::Create(SrcLoc loc) {
  NewTypeOperator op(loc);
  return op;
}

DeleteTypeOperator DeleteTypeOperator::Create(SrcLoc loc) {
  DeleteTypeOperator op(loc);
  return op;
}

TypeOperatorCollector::TypeOperatorCollector() {}

void TypeOperatorCollector::AddNew(SrcLoc inputLoc) {
  AddTypeOperator(NewTypeOperator::Create(inputLoc));
}
void TypeOperatorCollector::AddDelete(SrcLoc inputLoc) {
  AddTypeOperator(DeleteTypeOperator::Create(inputLoc));
}

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

FunctionTypeChunk FunctionTypeChunk::Create() {
  FunctionTypeChunk chunk;
  return chunk;
}

ParenTypeChunk ParenTypeChunk::Create(SrcLoc loc) {
  ParenTypeChunk chunk(loc);
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

void TypeChunkCollector::AddFunction() {
  AddTypeChunk(FunctionTypeChunk::Create());
}

TypeChunkCollector::TypeChunkCollector() {}

void TypeChunkCollector::AddParen(SrcLoc inputLoc) {
  AddTypeChunk(ParenTypeChunk::Create(inputLoc));
}

void TypeChunkCollector::AddPipe(SrcLoc inputLoc) {}

TypeChunkList::TypeChunkList(llvm::ArrayRef<TypeChunk> thunks) {
  std::uninitialized_copy(thunks.begin(), thunks.end(),
                          getTrailingObjects<TypeChunk>());
}

TypeChunkList *TypeChunkCollector::CreateTypeChunkList(ASTContext &astContext) {

  unsigned allocSize =
      TypeChunkList::totalSizeToAlloc<TypeChunk>(thunks.size());
  void *memPtr = astContext.AllocateMemory(allocSize, alignof(TypeChunkList));
  return new (memPtr) TypeChunkList(llvm::MutableArrayRef<TypeChunk>());
}

void TypeChunkCollector::Apply() {}

/// Apply the collected qualifiers to the given type.
Type TypeQualifierCollector::Apply(TypeBase *typePtr) { return Type(typePtr); }

/// Apply the collected qualifiers to the given type.
void TypeQualifierCollector::Apply(Type &ty) {

  // ty.SetFastQualifiers(GetFastQualifiers());
  if (HasConst()) {
    ty.AddConst();
  }
  if (HasPure()) {
    ty.AddPure();
  }
  if (HasFinal()) {
    ty.AddFinal();
  }
  if (HasMutable()) {
    ty.AddMutable();
  }
}

// unsigned TypeQualifierCollector::GetFastQualifiers() {

// }