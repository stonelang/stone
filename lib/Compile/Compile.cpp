#include "stone/Compile/Compile.h"
#include "stone/Compile/DebugLangListener.h"
#include "stone/Compile/Lang.h"
#include "stone/Compile/LangListener.h"
#include "stone/Compile/Parse.h"
#include "stone/Compile/TypeCheck.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMContext.h"
#include "stone/Core/LLVMInit.h"
#include "stone/Core/MainExecutablePath.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
#include "stone/Option/ModeKind.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/Module.h"

using namespace stone;

using stone::Lang;
using stone::LangListener;
using stone::ModeKind;
using stone::SyntaxListener;
using stone::syn::SyntaxFile;
using stone::syn::SyntaxFileKind;

int lang::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                  void *mainAddr, LangListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  auto Finish = [&](int status = 0) -> int {
    int err = 1;
    return status ? status : err;
  };

  if (args.empty()) {
    // ctx.Printd(SrcLoc(), diag::err_no_compile_args);
    return Finish(1);
  }

  std::unique_ptr<DebugLangListener> debugListener;
  Lang lang;

  STONE_DEFER { lang.Finish(); };
  lang.Initialize();

  if (listener) {
    lang.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugLangListener>();
    lang.SetListener(debugListener.get());
  }

  // Build up the context
  auto &lc = lang.GetLangContext();
  auto matinExecPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  lc.SetMainExecutablePath(matinExecPath);

  if (lc.ParseArgs(args) == stone::Err) {
    return Finish(1);
  }

  if (lc.GetMode().IsAlien()) {
    // lang.Printd(SrcLoc(), diags::err_alien_mode)
    Finish(1);
  }

  if (lc.GetMode().IsPrintHelp()) {
    lang.PrintHelp();
    return Finish();
  }
  if (lc.GetMode().IsPrintVersion()) {
    lang.PrintVersion();
    return Finish();
  }

  if (!lc.GetMode().CanCompile()) {
    /// lang.Printd()
    return Finish(1);
  }

  // lc.BuildSources();
  if (lang.GetListener()) {
    lang.GetListener()->OnCompileConfigured(lang);
  }

  lang.Compile();

  return Finish();
}

void Lang::Compile() {

  if (listener) {
    listener->OnCompileStarted(*this);
  }

  PerformCodeAnalysis();
  if (JustCodeAnalysis()) {
    // Do some things
    return;
  }
  PerformCodeGen();
}
