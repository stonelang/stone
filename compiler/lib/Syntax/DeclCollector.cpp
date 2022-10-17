#include "stone/Syntax/DeclCollector.h"

using namespace stone;
using namespace stone::syn;

DeclCollector::DeclCollector(AttributeFactory &attributeFactory)
    : attributeFactory(attributeFactory),
      typePatternCollector(typeSpecifierCollector) {}


void StorageSpecifierCollector::Apply() {}

void FunctionSpecifierCollector::Apply() {}

void AccessLevelCollector::Apply() {}