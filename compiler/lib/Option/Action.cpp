#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::file;
using namespace stone::opts;
using namespace llvm::opt;

ActionKind opts::GetActionKindByOptionID(const unsigned actionOptionID) {
  switch (actionOptionID) {
  case opts::Parse:
    return ActionKind::Parse;
  case opts::ResolveImports:
    return ActionKind::ResolveImports;
  case opts::DumpAST:
    return ActionKind::DumpAST;
  case opts::TypeCheck:
    return ActionKind::TypeCheck;
  case opts::PrintAST:
    return ActionKind::PrintAST;
  case opts::PrintIR:
    return ActionKind::PrintIR;
  case opts::EmitIRAfter:
    return ActionKind::EmitIRAfter;
  case opts::EmitIRBefore:
    return ActionKind::EmitIRBefore;
  case opts::EmitBC:
    return ActionKind::EmitBC;
  case opts::EmitObject:
    return ActionKind::EmitObject;
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

Action stone::ComputeAction(const llvm::opt::ArgList &args) {

  auto actionArg = args.getLastArg(opts::ModeGroup);
  if (!actionArg) {
    // We just default to emitting an object file since nothing was presented.
    return Action(ActionKind::None);
  }
  auto actionKind = opts::GetActionKindByOptionID(opts::GetArgID(actionArg));
  if (actionKind == ActionKind::Alien) {
    return Action(ActionKind::Alien);
  }
  return Action(actionKind, opts::GetArgName(actionArg));
}

FileType Action::GetOutputFileType() const {
  return Action::GetOutputFileTypeByActionKind(GetKind());
}

FileType Action::GetOutputFileTypeByActionKind(ActionKind kind) {
  switch (kind) {
  case ActionKind::None:
  case ActionKind::Parse:
  case ActionKind::ResolveImports:
  case ActionKind::TypeCheck:
  // case ActionKind::TypecheckModuleFromInterface:
  case ActionKind::DumpAST:
  // case ActionKind::DumpInterfaceHash:
  case ActionKind::PrintAST:
  case ActionKind::PrintIR:
  // case ActionKind::DumpScopeMaps:
  // case ActionKind::DumpTypeRefinementContexts:
  // case ActionKind::DumpIRTypeInfo:
  // case ActionKind::DumpPCM:
  case ActionKind::PrintVersion:
    // case ActionKind::Immediate:
    return FileType::None;

    // TODO: case ActionKind::EmitPCH:
    //   return FileType::PCH;

  case ActionKind::MergeModules:
  case ActionKind::EmitModule:
    // case ActionKind::CompileModuleFromInterface:
    return FileType::StoneModule;

  case ActionKind::EmitAssembly:
    return FileType::Assembly;

  // case ActionKind::EmitIRGen:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitIRBefore:
    return FileType::IR;

  case ActionKind::EmitBC:
    return FileType::BC;

  case ActionKind::EmitObject:
    return FileType::Object;

    // case ActionKind::EmitImportedModules:
    //   return FileType::ImportedModules;

    // case ActionKind::EmitPCM:
    //   return FileType::ClangModuleFile;

    // case ActionKind::ScanDependencies:
    //   return FileType::JSONDependencies;
    // case ActionKind::PrintFeature:
    //   return FileType::JSONFeatures;
  default:
    assert(false && "No file-type found for this particular mode-kind");
  }
}
