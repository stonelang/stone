#include "stone/Syntax/DeclSpecifier.h"

using namespace stone::syn;

PointerDeclaratorChunk PointerDeclaratorChunk::Create() {
  PointerDeclaratorChunk chunk;
  return chunk;
}

void PointerDeclaratorChunk::AddPointer() {
  // pointerCount++;
}

ReferenceDeclaratorChunk ReferenceDeclaratorChunk::Create() {
  ReferenceDeclaratorChunk chunk;
  return chunk;
}

void ReferenceDeclaratorChunk::AddReference() {
  // referenceCount++;
}
