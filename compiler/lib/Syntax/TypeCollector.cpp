#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeOperator.h"
#include "stone/Syntax/TypeQualifier.h"
#include "stone/Syntax/TypeThunk.h"

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

TypeThunkCollector::TypeThunkCollector() {}

void TypeThunkCollector::AddParen(SrcLoc inputLoc) {}

void TypeThunkCollector::AddPipe(SrcLoc inputLoc) {}

TypeThunkList::TypeThunkList(llvm::ArrayRef<TypeThunk> thunks) {
  std::uninitialized_copy(thunks.begin(), thunks.end(),
                          getTrailingObjects<TypeThunk>());
}

TypeThunkList *TypeThunkCollector::CreateTypeThunkList(ASTContext &astContext) {

  unsigned allocSize =
      TypeThunkList::totalSizeToAlloc<TypeThunk>(thunks.size());
  void *memPtr = astContext.Allocate(allocSize, alignof(TypeThunkList));
  return new (memPtr) TypeThunkList(llvm::MutableArrayRef<TypeThunk>());
}

void TypeThunkCollector::Apply() {}

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
  if (HasImmutable()) {
    ty.AddImmutable();
  }
  if (HasMutable()) {
    ty.AddMutable();
  }
}

// unsigned TypeQualifierCollector::GetFastQualifiers() {

// }