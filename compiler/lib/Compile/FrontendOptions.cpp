#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/FrontendOptionsConverter.h"
#include "stone/Context.h"

using namespace stone;

static Error ComputeFrontendOptions(
    llvm::opt::InputArgList &ial, DiagnosticEngine &de, LangOptions &langOpts,
    FrontendOptions &frontendOpts,
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  FrontendOptionsConverter converter(de, ial, langOpts, frontendOpts);

  return Error(converter.Convert(buffers));
}

static Error ComputeLangOptions(llvm::opt::InputArgList &ial,
                                DiagnosticEngine &de,
                                FrontendOptions &frontendOpts,
                                LangOptions &langOpts) {

  return Error();
}

static Error ComputeCodeGenOptions(llvm::opt::InputArgList &ial,
                                   DiagnosticEngine &de,
                                   FrontendOptions &frontendOpts,
                                   CodeGenOptions &codeGenOpts) {

  return Error();
}

static Error
ComputeTypeCheckerOptions(llvm::opt::InputArgList &ial, DiagnosticEngine &de,
                          FrontendOptions &frontendOpts,
                          sem::TypeCheckerOptions &typeCheckerOpts) {

  return Error();
}

static Error ComputeSearchPathOptions(llvm::opt::InputArgList &ial,
                                      DiagnosticEngine &de,
                                      FrontendOptions &frontendOpts,
                                      SearchPathOptions &searchPathOpts) {
  return Error();
}

Error Frontend::ComputeOptions(llvm::opt::InputArgList &ial) {

  frontendOpts = std::make_unique<FrontendOptions>(Mode::Create(ial));
  if (frontendOpts->GetMode().IsAlien()) {
    return Error(true);
  }
  auto frontendOptsErr =
      ComputeFrontendOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                             GetContext().GetLangOptions(), *frontendOpts,
                             nullptr /* pass null for now*/);
  if (frontendOptsErr.Has()) {
  }
  ComputeLangOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                     *frontendOpts, GetContext().GetLangOptions());

  ComputeTypeCheckerOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                            *frontendOpts, typeCheckerOpts);
  ComputeSearchPathOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                           *frontendOpts, searchPathOpts);
  ComputeCodeGenOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                        *frontendOpts, codeGenOpts);

  return Error();
}
