#include "stone/Syntax/Pattern.h"

using namespace stone::syn;

PointerTypePattern PointerTypePattern::Create(SrcLoc loc) {
  PointerTypePattern pattern(loc);
  return pattern;
}

ReferenceTypePattern ReferenceTypePattern::Create(SrcLoc loc) {
  ReferenceTypePattern pattern(loc);
  return pattern;
}

void TypePatternCollector::AddDirect(SrcLoc loc) {}

void TypePatternCollector::AddPointer(SrcLoc loc) {}

void TypePatternCollector::AddReference(SrcLoc loc) {}

void TypePatternCollector::AddArray(SrcLoc loc) {}

void TypePatternCollector::AddBlockPointer(SrcLoc loc) {}

void TypePatternCollector::AddParen(SrcLoc loc) {}

void TypePatternCollector::AddPipe(SrcLoc loc) {}
