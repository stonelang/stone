#include "stone/Compile/LangOptions.h"
#include "stone/Basic/Context.h"

using namespace stone;

LangOptions::LangOptions() {
  /// The default mode kind of the system
  defaultModeKind = ModeKind::EmitObject;
}

file::Type LangOptions::GetFileTypeByModeKind(ModeKind kind) {
  switch (kind) {
  case ModeKind::None:
  case ModeKind::Parse:
  case ModeKind::ResolveUsings:
  case ModeKind::TypeCheck:
  // case ModeKind::TypecheckModuleFromInterface:
  case ModeKind::DumpSyntax:
  // case ModeKind::DumpInterfaceHash:
  case ModeKind::PrintSyntax:
  // case ModeKind::DumpScopeMaps:
  // case ModeKind::DumpTypeRefinementContexts:
  // case ModeKind::DumpTypeInfo:
  // case ModeKind::DumpPCM:
  case ModeKind::PrintVersion:
    // case ModeKind::Immediate:
    return file::Type::None;

    // TODO: case ModeKind::EmitPCH:
    //   return file::Type::PCH;

  case ModeKind::MergeModules:
  case ModeKind::EmitModule:
    // case ModeKind::CompileModuleFromInterface:
    return file::Type::StoneModule;

  case ModeKind::EmitAssembly:
    return file::Type::Assembly;

  // case ModeKind::EmitIRGen:
  case ModeKind::EmitIR:
    return file::Type::IR;

  case ModeKind::EmitBC:
    return file::Type::BC;

  case ModeKind::EmitObject:
    return file::Type::Object;

    // case ModeKind::EmitImportedModules:
    //   return file::Type::ImportedModules;

    // case ModeKind::EmitPCM:
    //   return file::Type::ClangModuleFile;

    // case ModeKind::ScanDependencies:
    //   return file::Type::JSONDependencies;
    // case ModeKind::PrintFeature:
    //   return file::Type::JSONFeatures;
  }
  stone::Panic("No file-type found for this particular mode-kind");
}