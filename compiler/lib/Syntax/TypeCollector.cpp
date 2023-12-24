#include "stone/Syntax/TypeQualifier.h"
#include "stone/Syntax/TypeOperator.h"
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

void TypeThunkCollector::Apply() {}

