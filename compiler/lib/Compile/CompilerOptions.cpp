#include "stone/Compile/CompilerOptions.h"
#include "stone/Basic/FileType.h"

using namespace stone;
using namespace stone::file;

bool CompilerOptions::DoesActionNeedProperModuleName(CompilerAction action) {
  switch (action) {
  case CompilerAction::None:
  case CompilerAction::PrintVersion:
  case CompilerAction::PrintFeature:
  case CompilerAction::PrintHelp:
  case CompilerAction::PrintHelpHidden:
  case CompilerAction::Parse:
  case CompilerAction::PrintASTBefore:
  case CompilerAction::ResolveImports:
  case CompilerAction::TypeCheck:
  case CompilerAction::PrintASTAfter:
    return false;
  case CompilerAction::EmitIRBefore:
  case CompilerAction::EmitIRAfter:
  case CompilerAction::PrintIR:
  case CompilerAction::EmitBC:
  case CompilerAction::EmitModule:
  case CompilerAction::MergeModules:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
}

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
  case CompilerAction::MergeModules:
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
  case CompilerAction::MergeModules:
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
  case CompilerAction::MergeModules:
    return false;
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
}

bool CompilerOptions::DoesActionRequireStoneStandardLibrary(
    CompilerAction action) {

  switch (action) {
  case CompilerAction::None:
  case CompilerAction::PrintVersion:
  case CompilerAction::PrintHelp:
  case CompilerAction::PrintHelpHidden:
  case CompilerAction::PrintFeature:
  case CompilerAction::Parse:
    return false;
  case CompilerAction::ResolveImports:
  case CompilerAction::PrintASTBefore:
  case CompilerAction::TypeCheck:
  case CompilerAction::PrintASTAfter:
  case CompilerAction::EmitIRBefore:
  case CompilerAction::EmitIRAfter:
  case CompilerAction::PrintIR:
  case CompilerAction::EmitBC:
  case CompilerAction::EmitModule:
  case CompilerAction::MergeModules:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
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
  case CompilerAction::MergeModules:
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
  case CompilerAction::MergeModules:
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
  case CompilerAction::MergeModules:
    return FileType::StoneModule;
  case CompilerAction::EmitObject:
    return FileType::Object;
  case CompilerAction::EmitAssembly:
    return FileType::Assembly;
  }
  llvm_unreachable("Unhandled action");
}

llvm::StringRef CompilerOptions::GetActionString(CompilerAction action) {
  // TODO: It would be nice to pull this out of the OptTable -- atuo gen into a
  // file
  switch (action) {
  case CompilerAction::None:
    return "none";
  case CompilerAction::PrintVersion:
    return "version";
  case CompilerAction::PrintHelp:
    return "help";
  case CompilerAction::PrintHelpHidden:
    return "help-hidden";
  case CompilerAction::PrintFeature:
    return "feature";
  case CompilerAction::Parse:
    return "parse";
  case CompilerAction::ResolveImports:
    return "resolve-imports";
  case CompilerAction::PrintASTBefore:
    return "print-ast-before";
  case CompilerAction::TypeCheck:
    return "type-check";
  case CompilerAction::PrintASTAfter:
    return "print-ast-after";
  case CompilerAction::EmitIRBefore:
    return "emit-ir-before";
  case CompilerAction::EmitIRAfter:
    return "emit-ir-after";
  case CompilerAction::PrintIR:
    return "print-ir";
  case CompilerAction::EmitBC:
    return "emit-bc";
  case CompilerAction::EmitModule:
    return "emit-module";
  case CompilerAction::MergeModules:
    return "merge-modules";
  case CompilerAction::EmitObject:
    return "emit-object";
  case CompilerAction::EmitAssembly:
    return "emit-assembly";
  }
  llvm_unreachable("Unhandled action");
}

bool CompilerOptions::IsNoneAction() const {
  return !CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsPrintHelpAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsPrintHelpHiddenAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsPrintVersionAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsPrintFeatureAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsParseAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}

bool CompilerOptions::ShouldActionOnlyParse() const {
  return CompilerOptions::ShouldActionOnlyParse(mainAction);
}

bool CompilerOptions::IsResolveImportsAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsPrintASTBeforeAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsTypeCheckAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsPrintASTAfterAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsEmitIRAfterAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsEmitIRBeforeAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}

bool CompilerOptions::DoesActionGenerateIR() const {
  return CompilerOptions::DoesActionGenerateIR(mainAction);
}

bool CompilerOptions::DoesActionGenerateNative() const {
  return CompilerOptions::DoesActionGenerateNative(mainAction);
}

bool CompilerOptions::IsEmitModuleAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsEmitBCAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsEmitObjectAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}
bool CompilerOptions::IsEmitAssemblyAction() const {
  return CompilerOptions::IsAnyAction(mainAction);
}

bool CompilerOptions::IsAnyAction(CompilerAction action) {
  switch (action) {
  case CompilerAction::None:
    return false;
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
  case CompilerAction::MergeModules:
  case CompilerAction::EmitObject:
  case CompilerAction::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled action");
}
