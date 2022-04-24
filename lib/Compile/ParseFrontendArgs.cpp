#include "stone/Compile/Frontend.h"
#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/FrontendOptionsConverter.h"
#include "stone/Context.h"

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
  frontendOpts = std::make_unique<FrontendOptions>(ComputeMode(ial));

  auto err = ParseFrontendArgs(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                               GetContext().GetLangOptions(), *frontendOpts,
                               nullptr /* pass null for now*/);
  if (err.Has()) {
    // TODO:
  }
  ParseLangArgs(ial, GetContext().GetDiagUnit().GetDiagEngine(), *frontendOpts,
                GetContext().GetLangOptions());

  ParseTypeCheckerArgs(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                       *frontendOpts, typeCheckerOpts);
  ParseSearchPathArgs(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                      *frontendOpts, searchPathOpts);
  ParseCodeGenArgs(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                   *frontendOpts, codeGenOpts);

  return ial;
}

// stone::Error Frontend::ParseArguments(llvm::ArrayRef<const char *> args) {
//   return stone::Error();
// }
