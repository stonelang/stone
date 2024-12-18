#include "stone/Compile/CompilerOptions.h"
#include "stone/Parse/Lexer.h"

using namespace stone;

CompilerOptions::CompilerOptions() {
  excludedFlagsBitmask = opts::ExcludeCompilerOption;
}

bool CompilerOptions::DoesActionNeedProperModuleName(
    CompilerActionKind actionKind) {
  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintFeature:
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::Parse:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
    return false;
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::ShouldActionOnlyParse(CompilerActionKind actionKind) {
  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintFeature:
    return true;
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
    return false;
  case CompilerActionKind::Parse:
  case CompilerActionKind::EmitParse:
    return true;
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return false;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::DoesActionGenerateIR(CompilerActionKind actionKind) {

  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintFeature:
  case CompilerActionKind::Parse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
    return false;
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::DoesActionGenerateNativeCode(
    CompilerActionKind actionKind) {

  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintFeature:
  case CompilerActionKind::Parse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
    return false;
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::DoesActionRequireStoneStandardLibrary(
    CompilerActionKind actionKind) {

  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintFeature:
  case CompilerActionKind::Parse:
    return false;
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::DoesActionRequireInputs(CompilerActionKind actionKind) {

  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
    return false;
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
    return true;
  case CompilerActionKind::PrintFeature:
    return false;
  case CompilerActionKind::Parse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::DoesActionProduceOutput(CompilerActionKind actionKind) {
  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
    return false;
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintFeature:
  case CompilerActionKind::Parse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::DoesActionPerformEndOfPipelineActions(
    CompilerActionKind actionKind) {
  return true;
}

bool CompilerOptions::DoesActionSupportCompilationCaching(
    CompilerActionKind actionKind) {
  return true;
}

FileType
CompilerOptions::GetActionOutputFileType(CompilerActionKind actionKind) {

  switch (actionKind) {
  case CompilerActionKind::None:
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintFeature:
  case CompilerActionKind::Parse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
    return FileType::None;
  case CompilerActionKind::EmitIR:
    return FileType::None;
  case CompilerActionKind::EmitBC:
    return FileType::BC;
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
    return FileType::StoneModuleFile;
  case CompilerActionKind::EmitObject:
    return FileType::Object;
  case CompilerActionKind::EmitAssembly:
    return FileType::Assembly;
  }
  llvm_unreachable("Unhandled actionKind");
}

llvm::StringRef
CompilerOptions::GetActionString(CompilerActionKind actionKind) {
  // TODO: It would be nice to pull this out of the OptTable -- atuo gen into a
  // file
  switch (actionKind) {
  case CompilerActionKind::None:
    return "none";
  case CompilerActionKind::PrintVersion:
    return "version";
  case CompilerActionKind::PrintHelp:
    return "help";
  case CompilerActionKind::PrintHelpHidden:
    return "help-hidden";
  case CompilerActionKind::PrintFeature:
    return "feature";
  case CompilerActionKind::Parse:
    return "parse";
  case CompilerActionKind::ResolveImports:
    return "resolve-imports";
  case CompilerActionKind::EmitParse:
    return "emit-parse";
  case CompilerActionKind::TypeCheck:
    return "type-check";
  case CompilerActionKind::EmitAST:
    return "print-ast";
  case CompilerActionKind::EmitIR:
    return "emit-ir";
  case CompilerActionKind::EmitBC:
    return "emit-bc";
  case CompilerActionKind::EmitModule:
    return "emit-module";
  case CompilerActionKind::MergeModules:
    return "merge-modules";
  case CompilerActionKind::EmitObject:
    return "emit-object";
  case CompilerActionKind::EmitAssembly:
    return "emit-assembly";
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::IsNoneAction() const {
  return primaryActionKind == CompilerActionKind::None;
}
bool CompilerOptions::IsPrintHelpAction() const {
  return primaryActionKind == CompilerActionKind::PrintHelp;
}
bool CompilerOptions::IsPrintHelpHiddenAction() const {
  return primaryActionKind == CompilerActionKind::PrintHelpHidden;
}
bool CompilerOptions::IsPrintVersionAction() const {
  return primaryActionKind == CompilerActionKind::PrintVersion;
}
bool CompilerOptions::IsPrintFeatureAction() const {
  return primaryActionKind == CompilerActionKind::PrintFeature;
}
bool CompilerOptions::IsParseAction() const {
  return primaryActionKind == CompilerActionKind::Parse;
}

bool CompilerOptions::ShouldActionOnlyParse() const {
  return CompilerOptions::ShouldActionOnlyParse(primaryActionKind);
}

bool CompilerOptions::IsResolveImportsAction() const {
  return primaryActionKind == CompilerActionKind::ResolveImports;
}
bool CompilerOptions::IsEmitParseAction() const {
  return primaryActionKind == CompilerActionKind::EmitParse;
}
bool CompilerOptions::IsTypeCheckAction() const {
  return primaryActionKind == CompilerActionKind::TypeCheck;
}
bool CompilerOptions::IsEmitASTAction() const {
  return primaryActionKind == CompilerActionKind::EmitAST;
}
bool CompilerOptions::IsEmitIRAction() const {
  return primaryActionKind == CompilerActionKind::EmitIR;
}

bool CompilerOptions::DoesActionGenerateIR() const {
  return CompilerOptions::DoesActionGenerateIR(primaryActionKind);
}

bool CompilerOptions::DoesActionGenerateNativeCode() const {
  return CompilerOptions::DoesActionGenerateNativeCode(primaryActionKind);
}

bool CompilerOptions::IsEmitModuleAction() const {
  return primaryActionKind == CompilerActionKind::EmitModule;
}
bool CompilerOptions::IsEmitBCAction() const {
  return primaryActionKind == CompilerActionKind::EmitBC;
}
bool CompilerOptions::IsEmitObjectAction() const {
  return primaryActionKind == CompilerActionKind::EmitObject;
}
bool CompilerOptions::IsEmitAssemblyAction() const {
  return primaryActionKind == CompilerActionKind::EmitAssembly;
}

bool CompilerOptions::IsImmediateAction() const {
  switch (primaryActionKind) {
  case CompilerActionKind::None: // TODO : ??
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintFeature:
    return true;
  case CompilerActionKind::Parse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return false;
  }
  llvm_unreachable("Unhandled actionKind");
}

bool CompilerOptions::IsAnyAction(CompilerActionKind actionKind) {
  switch (actionKind) {
  case CompilerActionKind::None:
    return false;
  case CompilerActionKind::PrintVersion:
  case CompilerActionKind::PrintHelp:
  case CompilerActionKind::PrintHelpHidden:
  case CompilerActionKind::PrintFeature:
  case CompilerActionKind::Parse:
  case CompilerActionKind::ResolveImports:
  case CompilerActionKind::EmitParse:
  case CompilerActionKind::TypeCheck:
  case CompilerActionKind::EmitAST:
  case CompilerActionKind::EmitIR:
  case CompilerActionKind::EmitBC:
  case CompilerActionKind::EmitModule:
  case CompilerActionKind::MergeModules:
  case CompilerActionKind::EmitObject:
  case CompilerActionKind::EmitAssembly:
    return true;
  }
  llvm_unreachable("Unhandled actionKind");
}

Status CompilerOptions::IsValidModuleName(const llvm::StringRef moduleName) {
  if (!Lexer::isIdentifier(moduleName)) {
    return Status::Error();
  }
  return Status();
}
