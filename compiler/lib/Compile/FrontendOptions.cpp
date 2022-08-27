#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/FrontendOptionsConverter.h"
#include "stone/Context.h"

using namespace stone;

static Error ComputeFrontendOptions(
    llvm::opt::InputArgList &ial, DiagnosticEngine &de, LangOptions &langOpts,
    FrontendOptions &invocationOpts,
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  FrontendOptionsConverter converter(de, ial, langOpts, invocationOpts);

  return Error(converter.Convert(buffers));
}

static Error ComputeLangOptions(llvm::opt::InputArgList &ial,
                                DiagnosticEngine &de,
                                FrontendOptions &invocationOpts,
                                LangOptions &langOpts) {

  return Error();
}

static Error ComputeCodeGenOptions(llvm::opt::InputArgList &ial,
                                   DiagnosticEngine &de,
                                   FrontendOptions &invocationOpts,
                                   CodeGenOptions &codeGenOpts) {

  return Error();
}

static Error
ComputeTypeCheckerOptions(llvm::opt::InputArgList &ial, DiagnosticEngine &de,
                          FrontendOptions &invocationOpts,
                          sem::TypeCheckerOptions &typeCheckerOpts) {

  return Error();
}

static Error ComputeSearchPathOptions(llvm::opt::InputArgList &ial,
                                      DiagnosticEngine &de,
                                      FrontendOptions &invocationOpts,
                                      SearchPathOptions &searchPathOpts) {
  return Error();
}

Error CompilerInvocation::ComputeOptions(llvm::opt::InputArgList &ial) {

  invocationOpts = std::make_unique<FrontendOptions>(Mode::Create(ial));
  if (invocationOpts->GetMode().IsAlien()) {
    return Error(true);
  }
  auto invocationOptsErr =
      ComputeFrontendOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                             GetContext().GetLangOptions(), *invocationOpts,
                             nullptr /* pass null for now*/);
  if (invocationOptsErr.Has()) {
  }
  ComputeLangOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                     *invocationOpts, GetContext().GetLangOptions());

  ComputeTypeCheckerOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                            *invocationOpts, typeCheckerOpts);
  ComputeSearchPathOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                           *invocationOpts, searchPathOpts);
  ComputeCodeGenOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
                        *invocationOpts, codeGenOpts);

  return Error();
}
