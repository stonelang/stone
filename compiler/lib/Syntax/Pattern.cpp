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

DirectTypePattern DirectTypePattern::Create() {
  DirectTypePattern pattern;
  return pattern;
}

void TypePatternCollector::AddDirect() {
  AddTypePattern(DirectTypePattern::Create());
}

void TypePatternCollector::AddPointer(SrcLoc inputLoc) {
  AddTypePattern(PointerTypePattern::Create(inputLoc));
}

void TypePatternCollector::AddReference(SrcLoc inputLoc) {
  AddTypePattern(ReferenceTypePattern::Create(inputLoc));
}

void TypePatternCollector::AddArray(SrcLoc inputLoc) {}

void TypePatternCollector::AddBlockPointer(SrcLoc inputLoc) {}

void TypePatternCollector::AddParen(SrcLoc inputLoc) {}

void TypePatternCollector::AddPipe(SrcLoc inputLoc) {}
