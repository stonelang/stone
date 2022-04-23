#include "stone/Basic/Context.h"
#include "stone/Compile/Frontend.h"
#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/FrontendOptionsConverter.h"

using namespace stone;

FrontendOptions::FrontendOptions(const Mode &mode) : BaseOptions(mode) {}

static stone::Error ParseFrontendArgs(
    llvm::opt::InputArgList &ial, DiagnosticEngine &de, LangOptions &langOpts,
    FrontendOptions &frontendOpts,
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  FrontendOptionsConverter converter(de, ial, langOpts, frontendOpts);

  return stone::Error(converter.Convert(buffers));
}

static void ParseLangArgs(llvm::opt::InputArgList &ial, DiagnosticEngine &de,
                          FrontendOptions &frontendOpts,
                          LangOptions &langOpts) {}

static void ParseCodeGenArgs(llvm::opt::InputArgList &ial, DiagnosticEngine &de,
                             FrontendOptions &frontendOpts,
                             CodeGenOptions &codeGenOpts) {}

static void ParseTypeCheckerArgs(llvm::opt::InputArgList &ial,
                                 DiagnosticEngine &de,
                                 FrontendOptions &frontendOpts,
                                 TypeCheckerOptions &typeCheckerOpts) {}

static void ParseSearchPathArgs(llvm::opt::InputArgList &ial,
                                DiagnosticEngine &de,
                                FrontendOptions &frontendOpts,
                                SearchPathOptions &searchPathOpts) {}

llvm::opt::InputArgList &
Frontend::ParseArgs(llvm::ArrayRef<const char *> args) {

  auto &ial = Session::ParseArgs(args);

  // TODO: Check for Alien mode here.
  frontendOpts = std::make_unique<FrontendOptions>(GetMode());

  auto err = ParseFrontendArgs(ial, GetContext().GetDiagEngine(),
                               GetContext().GetLangOptions(), *frontendOpts,
                               nullptr /* pass null for now*/);
  if (err.Has()) {
    // TODO:
  }
  ParseLangArgs(ial, GetContext().GetDiagEngine(), *frontendOpts,
                GetContext().GetLangOptions());

  ParseTypeCheckerArgs(ial, GetContext().GetDiagEngine(), *frontendOpts,
                       typeCheckerOpts);
  ParseSearchPathArgs(ial, GetContext().GetDiagEngine(), *frontendOpts,
                      searchPathOpts);
  ParseCodeGenArgs(ial, GetContext().GetDiagEngine(), *frontendOpts,
                   codeGenOpts);

  return ial;
}

// stone::Error Frontend::ParseArguments(llvm::ArrayRef<const char *> args) {
//   return stone::Error();
// }

file::Type FrontendOptions::GetFileTypeByModeKind(ModeKind kind) {
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
