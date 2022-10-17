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

ValueTypePattern ValueTypePattern::Create() {
  ValueTypePattern pattern;
  return pattern;
}

void TypePatternCollector::AddValue() {
  AddTypePattern(ValueTypePattern::Create());
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

void TypePatternCollector::Apply() {}
