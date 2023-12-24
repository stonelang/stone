#include "stone/Syntax/DeclCollector.h"

using namespace stone;

DeclCollector::DeclCollector() {}

void DeclCollector::Apply(Decl *d) {

  GetTypeCollector().Apply();

  // // if (GetTypeQualifierCollector().HasAny()) {
  // //   GetTypeQualifierCollector().Apply();
  // // }
  // if (GetTypeThunkCollector().HasAny()) {
  //   GetTypeThunkCollector().Apply();
  // }
  // if (GetStorageSpecifierCollector().HasAny()) {
  //   GetStorageSpecifierCollector().Apply();
  // }
  // // FunctionSpecifierCollector functionSpecifierCollector;
  // if (GetAccessLevelCollector().HasAny()) {
  //   GetAccessLevelCollector().Apply();
  // }
}

void TypeCollector::Apply() {}

void StorageSpecifierCollector::Apply() {}

void FunctionSpecifierCollector::Apply() {}

void AccessLevelCollector::Apply() {}