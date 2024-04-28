#include "stone/AST/DeclSpecifier.h"

using namespace stone;

DeclSpecifierCollector::DeclSpecifierCollector() {}

void DeclSpecifierCollector::Apply(Decl *d) {

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

void StorageSpecifierCollector::Apply() {}

void FunctionSpecifierCollector::Apply() {}

void AccessSpecifierCollector::Apply() {}