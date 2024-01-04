#include "stone/Compile/CompilerOptions.h"
#include "stone/Basic/FileType.h"

using namespace stone;
using namespace stone::file;

bool CompilerOptions::ShouldActionOnlyParse(CompilerAction action) {
  switch (action) {
  case CompilerAction::None:
  case CompilerAction::PrintVersion:
  case CompilerAction::PrintFeature:
    return true;
  case CompilerAction::PrintHelp:
  case CompilerAction::PrintHelpHidden:
    return false;
  case CompilerAction::Parse:
  case CompilerAction::PrintASTBefore:
    return true;
  case CompilerAction::ResolveImports:
  case CompilerAction::TypeCheck:
  case CompilerAction::PrintASTAfter:
  case CompilerAction::EmitIRBefore:
  case CompilerAction::EmitIRAfter:
  case CompilerAction::PrintIR:
  case CompilerAction::EmitBC:
  case CompilerAction::EmitModule:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return false;
  }
  llvm_unreachable("Unhandled action");
}

bool CompilerOptions::DoesActionGenerateIR(CompilerAction action) {

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
    return false;
  case CompilerAction::EmitIRBefore:
  case CompilerAction::EmitIRAfter:
  case CompilerAction::PrintIR:
  case CompilerAction::EmitBC:
  case CompilerAction::EmitModule:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
}

bool CompilerOptions::DoesActionGenerateNative(CompilerAction action) {

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
  case CompilerAction::EmitModule:
    return false;
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
}

bool CompilerOptions::DoesActionRequireStoneStandardLibrary(
    CompilerAction action) {
  return true;
}

bool CompilerOptions::DoesActionRequireInputs(CompilerAction action) {

  switch (action) {
  case CompilerAction::None:
  case CompilerAction::PrintVersion:
    return false;
  case CompilerAction::PrintHelp:
  case CompilerAction::PrintHelpHidden:
    return true;
  case CompilerAction::PrintFeature:
    return false;
  case CompilerAction::Parse:
  case CompilerAction::ResolveImports:
  case CompilerAction::PrintASTBefore:
  case CompilerAction::TypeCheck:
  case CompilerAction::PrintASTAfter:
  case CompilerAction::EmitIRBefore:
  case CompilerAction::EmitIRAfter:
  case CompilerAction::PrintIR:
  case CompilerAction::EmitBC:
  case CompilerAction::EmitModule:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
}

bool CompilerOptions::DoesActionProduceOutput(CompilerAction action) {
  switch (action) {
  case CompilerAction::None:
  case CompilerAction::PrintVersion:
    return false;
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
  case CompilerAction::EmitModule:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
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
    return FileType::None;
  case CompilerAction::EmitIRBefore:
  case CompilerAction::EmitIRAfter:
    return FileType::IR;
  case CompilerAction::PrintIR:
    return FileType::None;
  case CompilerAction::EmitBC:
    return FileType::BC;
  case CompilerAction::EmitModule:
    return FileType::StoneModule;
  case CompilerAction::EmitObject:
    return FileType::Object;
  case CompilerAction::EmitAssembly:
    return FileType::Assembly;
  }
  llvm_unreachable("Unhandled action");
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
bool CompilerOptions::IsEmitBCAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitObjectAction() const {
  return CompilerOptions::IsAnyAction(action);
}
bool CompilerOptions::IsEmitAssemblyAction() const {
  return CompilerOptions::IsAnyAction(action);
}

bool CompilerOptions::IsAnyAction(CompilerAction action) {
  switch (action) {
  case CompilerAction::None:
    false;
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
  case CompilerAction::EmitModule:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
}
