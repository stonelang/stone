#include "stone/Option/Action.h"
#include "stone/Option/Options.h"
#include "stone/Public.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;
using namespace llvm::opt;

ActionKind opts::GetActionKindByOptionID(const unsigned actionOptionID) {
  switch (actionOptionID) {
  case opts::Parse:
    return ActionKind::Parse;
  case opts::ResolveImports:
    return ActionKind::ResolveImports;
  case opts::DumpSyntax:
    return ActionKind::DumpSyntax;
  case opts::TypeCheck:
    return ActionKind::TypeCheck;
  case opts::PrintSyntax:
    return ActionKind::PrintSyntax;
  case opts::PrintIR:
    return ActionKind::PrintIR;
  case opts::EmitIRAfter:
    return ActionKind::EmitIRAfter;
  case opts::EmitIRBefore:
    return ActionKind::EmitIRBefore;
  case opts::EmitBC:
    return ActionKind::EmitBC;
  case opts::EmitObject:
    ActionKind::EmitObject;
  case opts::EmitAssembly:
    return ActionKind::EmitAssembly;
  case opts::EmitLibrary:
    return ActionKind::EmitLibrary;
  case opts::EmitModule:
    return ActionKind::EmitModule;
  case opts::PrintVersion:
    return ActionKind::PrintVersion;
  case opts::PrintHelp:
    return ActionKind::PrintVersion;
  default:
    return ActionKind::Alien;
  }
}

file::Type Action::GetOutputFileType() const {
  return Action::GetOutputFileTypeByActionKind(GetKind());
}

file::Type Action::GetOutputFileTypeByActionKind(ActionKind kind) {
  switch (kind) {
  case ActionKind::None:
  case ActionKind::Parse:
  case ActionKind::ResolveImports:
  case ActionKind::TypeCheck:
  // case ActionKind::TypecheckModuleFromInterface:
  case ActionKind::DumpSyntax:
  // case ActionKind::DumpInterfaceHash:
  case ActionKind::PrintSyntax:
  case ActionKind::PrintIR:
  // case ActionKind::DumpScopeMaps:
  // case ActionKind::DumpTypeRefinementContexts:
  // case ActionKind::DumpTypeInfo:
  // case ActionKind::DumpPCM:
  case ActionKind::PrintVersion:
    // case ActionKind::Immediate:
    return file::Type::None;

    // TODO: case ActionKind::EmitPCH:
    //   return file::Type::PCH;

  case ActionKind::MergeModules:
  case ActionKind::EmitModule:
    // case ActionKind::CompileModuleFromInterface:
    return file::Type::StoneModule;

  case ActionKind::EmitAssembly:
    return file::Type::Assembly;

  // case ActionKind::EmitIRGen:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitIRBefore:
    return file::Type::IR;

  case ActionKind::EmitBC:
    return file::Type::BC;

  case ActionKind::EmitObject:
    return file::Type::Object;

    // case ActionKind::EmitImportedModules:
    //   return file::Type::ImportedModules;

    // case ActionKind::EmitPCM:
    //   return file::Type::ClangModuleFile;

    // case ActionKind::ScanDependencies:
    //   return file::Type::JSONDependencies;
    // case ActionKind::PrintFeature:
    //   return file::Type::JSONFeatures;
  }
  assert(false && "No file-type found for this particular mode-kind");
}

Action::Action() {}

file::Type action::GetActionKindOutputFileType(ActionKind kind) {
  switch (kind) {
  case ActionKind::None:
  case ActionKind::Parse:
  case ActionKind::ResolveImports:
  case ActionKind::TypeCheck:
  // case ActionKind::TypecheckModuleFromInterface:
  case ActionKind::DumpSyntax:
  // case ActionKind::DumpInterfaceHash:
  case ActionKind::PrintSyntax:
  case ActionKind::PrintIR:
  // case ActionKind::DumpScopeMaps:
  // case ActionKind::DumpTypeRefinementContexts:
  // case ActionKind::DumpTypeInfo:
  // case ActionKind::DumpPCM:
  case ActionKind::PrintVersion:
    // case ActionKind::Immediate:
    return file::Type::None;

    // TODO: case ActionKind::EmitPCH:
    //   return file::Type::PCH;

  case ActionKind::MergeModules:
  case ActionKind::EmitModule:
    // case ActionKind::CompileModuleFromInterface:
    return file::Type::StoneModule;

  case ActionKind::EmitAssembly:
    return file::Type::Assembly;

  // case ActionKind::EmitIRGen:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitIRBefore:
    return file::Type::IR;

  case ActionKind::EmitBC:
    return file::Type::BC;

  case ActionKind::EmitObject:
    return file::Type::Object;

    // case ActionKind::EmitImportedModules:
    //   return file::Type::ImportedModules;

    // case ActionKind::EmitPCM:
    //   return file::Type::ClangModuleFile;

    // case ActionKind::ScanDependencies:
    //   return file::Type::JSONDependencies;
    // case ActionKind::PrintFeature:
    //   return file::Type::JSONFeatures;
  }
  assert(false && "No file-type found for this particular mode-kind");
}

bool action::IsOutputable(ActionKind kind) {
  switch (kind) {
  case ActionKind::DumpSyntax:
  case ActionKind::PrintSyntax:
  case ActionKind::EmitIRBefore:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitBC:
  case ActionKind::EmitObject:
  case ActionKind::EmitAssembly:
  case ActionKind::EmitModule:
  case ActionKind::EmitLibrary:
    return true;
  default:
    return false;
  }
}
bool action::IsCompilable(ActionKind kind) {
  switch (kind) {
  case ActionKind::None:
  case ActionKind::Parse:
  case ActionKind::ResolveImports:
  case ActionKind::DumpSyntax:
  case ActionKind::TypeCheck:
  case ActionKind::PrintSyntax:
  case ActionKind::EmitIRBefore:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitBC:
  case ActionKind::EmitObject:
  case ActionKind::EmitAssembly:
  case ActionKind::EmitModule:
  case ActionKind::EmitLibrary:
    return true;
  default:
    return false;
  }
}

bool action::IsEmittable(ActionKind kind) {
  switch (kind) {
  case ActionKind::None:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitIRBefore:
  case ActionKind::EmitBC:
  case ActionKind::EmitObject:
  case ActionKind::EmitAssembly:
  case ActionKind::EmitModule:
  case ActionKind::EmitLibrary:
    return true;
  default:
    return false;
  }
}

bool action::IsSupport(ActionKind kind) {
  switch (kind) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
  case ActionKind::PrintVersion:
    return true;
  default:
    return false;
  }
}

// bool action::IsNone(ActionKind kind)  { return kind == ActionKind::None; }
// bool action::IsPrintHelp(ActionKind kind)  {
//   return kind == ActionKind::PrintHelp;
// }
// bool action::IsPrintHelpHidden(ActionKind kind)  {
//   return kind == ActionKind::PrintHelpHidden;
// }
// bool action::IsPrintVersion(ActionKind kind)  {
//   return kind == ActionKind::PrintVersion;
// }
// bool action::IsParse(ActionKind kind)  {
//   return kind == ActionKind::Parse;
// }
// bool action::IsResolveImports(ActionKind kind)  {
//   return kind == ActionKind::ResolveImports;
// }
// bool action::IsDumpSyntax(ActionKind kind)  {
//   return kind == ActionKind::DumpSyntax;
// }
// bool action::IsTypeCheck(ActionKind kind)  {
//   return kind == ActionKind::TypeCheck;
// }
// bool action::IsDumpTypeInfo(ActionKind kind)  {
//   return kind == ActionKind::DumpTypeInfo;
// }
// bool action::IsPrintSyntax(ActionKind kind)  {
//   return kind == ActionKind::PrintSyntax;
// }
// bool action::IsEmitIRAfter(ActionKind kind)  {
//   return kind == ActionKind::EmitIRAfter;
// }
// bool action::IsEmitIRBefore(ActionKind kind)  {
//   return kind == ActionKind::EmitIRBefore;
// }
// bool action::IsEmitIR(ActionKind kind)  {
//   return (action::IsEmitIRAfter() || action::IsEmitIRBefore());
// }
// bool action::IsEmitModule(ActionKind kind)  {
//   return kind == ActionKind::EmitModule;
// }
// bool action::IsEmitLibrary()  { return kind == ActionKind::EmitLibrary; }
// bool action::IsEmitBC(ActionKind kind) { return kind == ActionKind::EmitBC; }
// bool action::IsEmitObject(ActionKind kind)  {
//   return kind == ActionKind::EmitObject;
// }
// bool action::IsEmitAssembly(ActionKind kind)  {
//   return kind == ActionKind::EmitAssembly;
// }
// bool action::IsAlien(ActionKind kind)  {
//   return kind == ActionKind::Alien;
// }