#include "stone/Syntax/DeclCollector.h"

using namespace stone;
using namespace stone::syn;

DeclCollector::DeclCollector(AttributeFactory &attributeFactory)
    : attributeFactory(attributeFactory) {}

void DeclCollector::Apply() {

  // if (GetTypeCollector().GetTypeSpecifierCollector().HasAny()) {
  //   GetTypeSpecifierCollector().Apply();
  // }
  // // if (GetTypeQualifierCollector().HasAny()) {
  // //   GetTypeQualifierCollector().Apply();
  // // }
  // if (GetTypeChunkCollector().HasAny()) {
  //   GetTypeChunkCollector().Apply();
  // }
  // if (GetStorageSpecifierCollector().HasAny()) {
  //   GetStorageSpecifierCollector().Apply();
  // }
  // // FunctionSpecifierCollector functionSpecifierCollector;
  // if (GetAccessLevelCollector().HasAny()) {
  //   GetAccessLevelCollector().Apply();
  // }
}

void StorageSpecifierCollector::Apply() {}

void FunctionSpecifierCollector::Apply() {}

void AccessLevelCollector::Apply() {}