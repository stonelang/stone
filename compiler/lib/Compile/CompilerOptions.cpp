#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Public.h"

using namespace stone;

static Error ComputeCompilerOptions(
    llvm::opt::InputArgList &ial, DiagnosticEngine &de, LangOptions &langOpts,
    CompilerOptions &compilerOpts,
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  CompilerOptionsConverter converter(de, ial, langOpts, compilerOpts);

  return Error(converter.Convert(buffers));
}

static Error ComputeLangOptions(llvm::opt::InputArgList &ial,
                                DiagnosticEngine &de,
                                CompilerOptions &compilerOpts,
                                LangOptions &langOpts) {

  return Error();
}

static Error ComputeCodeGenOptions(llvm::opt::InputArgList &ial,
                                   DiagnosticEngine &de,
                                   CompilerOptions &compilerOpts,
                                   CodeGenOptions &codeGenOpts) {

  return Error();
}

static Error ComputeTypeCheckerOptions(llvm::opt::InputArgList &ial,
                                       DiagnosticEngine &de,
                                       CompilerOptions &compilerOpts,
                                       TypeCheckerOptions &typeCheckerOpts) {

  return Error();
}

static Error ComputeSearchPathOptions(llvm::opt::InputArgList &ial,
                                      DiagnosticEngine &de,
                                      CompilerOptions &compilerOpts,
                                      SearchPathOptions &searchPathOpts) {
  return Error();
}

Error CompilerInvocation::ComputeOptions(llvm::opt::InputArgList &ial) {

  compilerOpts = std::make_unique<CompilerOptions>(Mode::Create(ial));
  if (compilerOpts->GetMode().IsAlien()) {
    return Error(true);
  }
  auto compilerOptsErr = ComputeCompilerOptions(
      ial, GetLangContext().GetDiagUnit().GetDiagEngine(),
      GetLangContext().GetLangOptions(), *compilerOpts,
      nullptr /* pass null for now*/);
  if (compilerOptsErr.Has()) {
  }
  ComputeLangOptions(ial, GetLangContext().GetDiagUnit().GetDiagEngine(),
                     *compilerOpts, GetLangContext().GetLangOptions());

  ComputeTypeCheckerOptions(ial, GetLangContext().GetDiagUnit().GetDiagEngine(),
                            *compilerOpts, typeCheckerOpts);
  ComputeSearchPathOptions(ial, GetLangContext().GetDiagUnit().GetDiagEngine(),
                           *compilerOpts, searchPathOpts);
  ComputeCodeGenOptions(ial, GetLangContext().GetDiagUnit().GetDiagEngine(),
                        *compilerOpts, codeGenOpts);

  return Error();
}
