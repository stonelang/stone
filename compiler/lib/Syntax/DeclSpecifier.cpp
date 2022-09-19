#include "stone/Syntax/DeclSpecifier.h"

using namespace stone::syn;

PointerDeclarator PointerDeclarator::Create() {
  PointerDeclarator declarator;
  return declarator;
}

void PointerDeclarator::AddPointer() {
  // pointerCount++;
}

ReferenceDeclarator ReferenceDeclarator::Create() {
  ReferenceDeclarator declarator;
  return declarator;
}

void ReferenceDeclarator::AddReference() {
  // referenceCount++;
}
