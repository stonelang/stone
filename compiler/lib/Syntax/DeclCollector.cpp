#include "stone/Syntax/DeclCollector.h"

using namespace stone;
using namespace stone::syn;

DeclCollector::DeclCollector(AttributeFactory &attributeFactory)
    : attributeFactory(attributeFactory),
      typePatternCollector(typeSpecifierCollector) {}

void DeclCollector::Apply() {

  if (GetTypeSpecifierCollector().HasAny()) {
    GetTypeSpecifierCollector().Apply();
  }
  // if (GetTypeQualifierCollector().HasAny()) {
  //   GetTypeQualifierCollector().Apply();
  // }
  if (GetTypePatternCollector().HasAny()) {
    GetTypePatternCollector().Apply();
  }
  if (GetStorageSpecifierCollector().HasAny()) {
    GetStorageSpecifierCollector().Apply();
  }
  // FunctionSpecifierCollector functionSpecifierCollector;
  if (GetAccessLevelCollector().HasAny()) {
    GetAccessLevelCollector().Apply();
  }
}

void StorageSpecifierCollector::Apply() {}

void FunctionSpecifierCollector::Apply() {}

void AccessLevelCollector::Apply() {}