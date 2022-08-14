#include "stone/Diag/DiagUnit.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/CoreDiagnostic.h"

using namespace stone;

DiagUnit::DiagUnit(SrcMgr &sm) : sm(sm), de(GetDiagOptions(), sm) {}

DiagUnit::~DiagUnit() {}
