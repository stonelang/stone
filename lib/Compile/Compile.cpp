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

  std::unique_ptr<DebugLangListener> debugListener;

  Lang lang;
  STONE_DEFER { lang.Finish(); };
  lang.Initialize();

  if (args.empty()) {
    // lang.Printd(SrcLoc(), diag::err_no_compile_args);
    return Finish(1);
  }

  if (listener) {
    lang.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugLangListener>();
    lang.SetListener(debugListener.get());
  }

  auto &frontend = lang.GetFrontend();
  auto matinExecPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  frontend.SetMainExecutablePath(matinExecPath);

  auto &ial = frontend.ParseArgs(args);
  if (frontend.HasError()) {
    return Finish(1);
  }
  auto &mode = frontend.CreateMode(ial);
  if (mode.IsAlien()) {
    // lang.Printd(SrcLoc(), diags::err_alien_mode)
    Finish(1);
  }
  if (mode.IsPrintHelp()) {
    // lang.PrintHelp();
    return Finish();
  }
  if (mode.IsPrintVersion()) {
    lang.PrintVersion();
    return Finish();
  }
  if (!mode.CanCompile()) {
    /// lang.Printd()
    return Finish(1);
  }
  auto inputs = frontend.BuildInputFiles(ial);

  // auto sourceIDs = frontend.BuildSourceIDs(inputs);

  // lang.Compile(sourceIDs);

  // if (lang.GetListener()) {
  //   lang.GetListener()->OnCompileConfigured(lang);
  // }
  // // Pass
  // lang.Compile();

  return Finish();
}

void Lang::Compile(llvm::ArrayRef<const unsigned> sourceIDs) {

  if (listener) {
    listener->OnCompileStarted(*this);
  }
  PerformCodeAnalysis(sourceIDs);
  if (JustCodeAnalysis()) {
    // Do some things
    return;
  }
  PerformCodeGen();
}
