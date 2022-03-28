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
#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Core/TextDiagnosticFormatter.h"
#include "stone/Core/TextDiagnosticListener.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
#include "stone/Session/ModeKind.h"
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
    // lang.PrintD(SrcLoc(), diag::err_no_compile_args);
    return Finish(1);
  }

  if (listener) {
    lang.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugLangListener>();
    lang.SetListener(debugListener.get());
  }
  auto &frontend = lang.GetFrontend();
  auto mainExecPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  frontend.SetMainExecutablePath(mainExecPath);

  // Setup the dianostics formatter an emitter
  TextDiagnosticFormatter formatter;

  TextDiagnosticEmitter emitter;
  emitter.SetFormatter(std::move(formatter));

  TextDiagnosticListener textDiagListener;
  textDiagListener.SetEmitter(std::move(emitter));

  // Add the diagnostic listener
  frontend.GetContext().GetDiagEngine().AddListener(textDiagListener);

  auto &ial = frontend.ParseArgs(args);
  if (frontend.HasError()) {
    return Finish(1);
  }
  auto &mode = frontend.ComputeMode(ial);
  if (mode.IsAlien()) {
    // lang.PrintD(SrcLoc(), diags::err_alien_mode)
    Finish(1);
  }
  if (mode.IsPrintHelp()) {
    frontend.PrintHelp(frontend.GetOpts());
    return Finish();
  }
  if (mode.IsPrintVersion()) {
    lang.PrintVersion();
    return Finish();
  }
  if (!mode.CanCompile()) {
    /// lang.PrintD()
    return Finish(1);
  }
  auto inputs = frontend.BuildInputFiles(ial);
  if (frontend.HasError()) {
    return Finish(1);
  }
  auto sources = frontend.BuildSources(inputs);
  if (frontend.HasError()) {
    return Finish(1);
  }
  if (lang.GetListener()) {
    lang.GetListener()->OnCompileConfigured(lang);
  }
  lang.Compile(sources);
  if (frontend.HasError()) {
    return Finish(1);
  }

  return Finish();
}

void Lang::Compile(llvm::ArrayRef<SourceUnit *> sources) {
  if (listener) {
    listener->OnCompileStarted(*this);
  }
  // Create SyntaxFiles and perform type-checking on them
  PerformAnalysis(sources);
  if (frontend.JustAnalysis()) {
    // Do some things
    return;
  }
  // At this point, we should have a module with one or more syntax files
  PerformCodeGen();
}
