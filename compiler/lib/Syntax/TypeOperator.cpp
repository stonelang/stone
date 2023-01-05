#include "stone/Syntax/TypeOperator.h"
#include "stone/Syntax/SyntaxContext.h"

using namespace stone::syn;

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

TypeOperatorList::TypeOperatorList(llvm::ArrayRef<TypeOperator> ops) {
  std::uninitialized_copy(ops.begin(), ops.end(),
                          getTrailingObjects<TypeOperator>());
}

TypeOperatorList *TypeOperatorList::Create(llvm::ArrayRef<TypeOperator> ops,
                                           SyntaxContext &sc) {

  unsigned sizeToAlloc =
      TypeOperatorList::totalSizeToAlloc<TypeOperator>(ops.size());
  void *memPtr = sc.Allocate(sizeToAlloc, alignof(TypeOperatorList));
  return new (memPtr) TypeOperatorList(llvm::MutableArrayRef<TypeOperator>());
}
