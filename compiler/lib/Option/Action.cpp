#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

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
  default:
    assert(false && "No file-type found for this particular mode-kind");
  }
}

Action Action::Compute(const llvm::opt::ArgList &args) {

  auto actionArg = args.getLastArg(opts::ModeGroup);
  if (actionArg) {
    auto actionKind = opts::GetActionKindByOptionID(opts::GetArgID(actionArg));
    if (actionKind == ActionKind::Alien) {
      return Action(ActionKind::Alien);
    }
    return Action(actionKind, opts::GetArgName(actionArg));
  }
  // We just default to emitting an object file since nothing was presented.
  return Action(ActionKind::None);
}
