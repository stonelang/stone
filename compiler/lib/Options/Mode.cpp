#include "stone/Session/Mode.h"
#include "stone/Public.h"
#include "stone/Session/Options.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;
using namespace llvm::opt;

// static ModeKind ComputeModeKind(const llvm::opt::ArgList &args) {}

static ModeKind GetModeKind(const unsigned modeID) {

  // TODO: may have to claim
  switch (modeID) {
  case opts::Parse:
    return ModeKind::Parse;
  case opts::DumpAST:
    return ModeKind::DumpAST;
  case opts::TypeCheck:
    return ModeKind::TypeCheck;
  case opts::PrintAST:
    return ModeKind::PrintAST;
  case opts::PrintIR:
    return ModeKind::PrintIR;
  case opts::EmitIR:
    return ModeKind::EmitIR;
  case opts::EmitBC:
    return ModeKind::EmitBC;
  case opts::EmitObject:
    ModeKind::EmitObject;
  case opts::EmitAssembly:
    return ModeKind::EmitAssembly;
  case opts::EmitLibrary:
    return ModeKind::EmitLibrary;
  case opts::EmitModule:
    return ModeKind::EmitModule;
  case opts::PrintVersion:
    return ModeKind::PrintVersion;
  case opts::PrintHelp:
    return ModeKind::PrintVersion;
  default:
    return ModeKind::Alien;
  }
}

static llvm::StringRef GetModeName(ModeKind kind) {
  switch (kind) {
  case ModeKind::Parse:
    return "parse";
  default:
  }
}

Mode::Mode(ModeKind kind, llvm::StringRef name)
    : kind(kind), name(name),  timer(new stone::Timer(GetModeName(kind), "mode timer")) {}
Mode::~Mode() {}


std::unique_ptr<Mode> Mode::Create(const llvm::opt::InputArgList &ial) {
  auto modeArg = ial.getLastArg(opts::ModeGroup);
  if (modeArg) {
    auto modeKind = GetModeKind(modeArg->getOption().getID());
    if (modeKind == ModeKind::Alien) {
      return std::make_unique<Mode>(ModeKind::Alien);
    }
    return std::make_unique<Mode>(modeKind, modeArg->getOption().getName());
  }
  return std::make_unique<Mode>(ModeKind::None);
}

file::Type Mode::GetOutputFileType() const {
  return Mode::GetOutputFileTypeByModeKind(GetKind());
}

file::Type Mode::GetOutputFileTypeByModeKind(ModeKind kind) {
  switch (kind) {
  case ModeKind::None:
  case ModeKind::Parse:
  case ModeKind::ResolveImports:
  case ModeKind::TypeCheck:
  // case ModeKind::TypecheckModuleFromInterface:
  case ModeKind::DumpAST:
  // case ModeKind::DumpInterfaceHash:
  case ModeKind::PrintAST:
  case ModeKind::PrintIR:
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
  assert(false && "No file-type found for this particular mode-kind");
}
