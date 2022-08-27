#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Context.h"

using namespace stone;

static Error ComputeCompilerOptions(
    llvm::opt::InputArgList &ial, DiagnosticEngine &de, LangOptions &langOpts,
    CompilerOptions &invocationOpts,
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  CompilerOptionsConverter converter(de, ial, langOpts, invocationOpts);

  return Error(converter.Convert(buffers));
}

static Error ComputeLangOptions(llvm::opt::InputArgList &ial,
                                DiagnosticEngine &de,
                                CompilerOptions &invocationOpts,
                                LangOptions &langOpts) {

  return Error();
}

static Error ComputeCodeGenOptions(llvm::opt::InputArgList &ial,
                                   DiagnosticEngine &de,
                                   CompilerOptions &invocationOpts,
                                   CodeGenOptions &codeGenOpts) {

  return Error();
}

static Error
ComputeTypeCheckerOptions(llvm::opt::InputArgList &ial, DiagnosticEngine &de,
                          CompilerOptions &invocationOpts,
                          sem::TypeCheckerOptions &typeCheckerOpts) {

  return Error();
}

static Error ComputeSearchPathOptions(llvm::opt::InputArgList &ial,
                                      DiagnosticEngine &de,
                                      CompilerOptions &invocationOpts,
                                      SearchPathOptions &searchPathOpts) {
  return Error();
}

Error CompilerInvocation::ComputeOptions(llvm::opt::InputArgList &ial) {

  invocationOpts = std::make_unique<CompilerOptions>(Mode::Create(ial));
  if (invocationOpts->GetMode().IsAlien()) {
    return Error(true);
  }
  auto invocationOptsErr =
      ComputeCompilerOptions(ial, GetContext().GetDiagUnit().GetDiagEngine(),
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
