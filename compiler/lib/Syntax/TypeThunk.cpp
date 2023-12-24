#include "stone/Syntax/TypeThunk.h"
#include "stone/Syntax/ASTContext.h"

using namespace stone;

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
