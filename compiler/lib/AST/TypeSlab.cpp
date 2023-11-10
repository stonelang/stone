#include "stone/AST/TypeSlab.h"
#include "stone/AST/ASTContext.h"

using namespace stone;

void TypeSlabCollector::AddValue() { AddTypeSlab(TypeSlab(TypeSlabKind::Value, SrcLoc())); }

void TypeSlabCollector::AddPointer(SrcLoc inputLoc) {
  AddTypeSlab(TypeSlab(TypeSlabKind::Pointer, inputLoc));
}
void TypeSlabCollector::AddReference(SrcLoc inputLoc) {
   AddTypeSlab(TypeSlab(TypeSlabKind::Reference, inputLoc));
}
void TypeSlabCollector::AddArray(SrcLoc inputLoc) {
  AddTypeSlab(TypeSlab(TypeSlabKind::Array, inputLoc));
}

void TypeSlabCollector::AddMemberPointer(SrcLoc inputLoc) {
  AddTypeSlab(TypeSlab(TypeSlabKind::MemberPointer, inputLoc));
}

void TypeSlabCollector::AddParen(SrcLoc inputLoc) {}

void TypeSlabCollector::AddPipe(SrcLoc inputLoc) {}

void TypeSlabCollector::Apply() {}


TypeSlabs TypeSlabCollector::ComputeSlabs() {
  TypeSlabs slabs;
  return slabs;
}
