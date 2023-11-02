#include "stone/AST/DeclCollector.h"

using namespace stone;
using namespace stone::ast;

DeclCollector::DeclCollector() {}

void DeclCollector::Apply() {

  // GetTypeCollector().Apply();

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

// void TypeCollector::Apply() {}

// void StorageSpecifierCollector::Apply() {}

// void FunctionSpecifierCollector::Apply() {}

// void AccessLevelCollector::Apply() {}