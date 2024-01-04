#include "stone/Compile/CompilerOptions.h"
#include "stone/Basic/FileType.h"


using namespace stone;
using namespace stone::file;


bool CompilerOptions::ShouldActionOnlyParse(CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionGenerateOutput(CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionGenerateIR(CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionGenerateNative(CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionRequireStoneStandardLibrary(
    CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionRequireInputs(CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionProduceOutput(CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionPerformEndOfPipelineActions(
    CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionSupportCompilationCaching(
    CompilerAction action) {
  return true;
}

FileType CompilerOptions::GetActionOutputFileType(CompilerAction action) {
  return FileType::None;
}

bool CompilerOptions::IsNoneAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsPrintHelpAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsPrintHelpHiddenAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsPrintVersionAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsPrintFeatureAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsParseAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsResolveImportsAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsPrintASTBeforeAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsTypeCheckAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsPrintASTAfterAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitIRAfterAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitIRBeforeAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitModuleAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitLibraryAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitBCAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitObjectAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitAssemblyAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsAlienAction() const {
  return CompilerOptions::IsAlienAction(action);
}

bool CompilerOptions::IsAlienAction(CompilerAction action) {
  return action == CompilerAction::Alien;
}

bool CompilerOptions::IsAnyAction(CompilerAction action) {
  switch (action) {
  case CompilerAction::None:
  case CompilerAction::PrintVersion:
  case CompilerAction::PrintHelp:
  case CompilerAction::PrintHelpHidden:
  case CompilerAction::PrintFeature:
  case CompilerAction::Parse:
  case CompilerAction::ResolveImports:
  case CompilerAction::PrintASTBefore:
  case CompilerAction::TypeCheck:
  case CompilerAction::PrintASTAfter:
  case CompilerAction::EmitIRBefore:
  case CompilerAction::EmitIRAfter:
  case CompilerAction::PrintIR:
  case CompilerAction::EmitBC:
  case CompilerAction::EmitLibrary:
  case CompilerAction::EmitModule:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
  case CompilerAction::MergeModules:
    return true;
  default:
    return false;
  }
  llvm_unreachable("Unhandled action");
}
