#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Basic/TextDiagnosticFormatter.h"
#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Compile/DebugLangListener.h"
#include "stone/Compile/LangInstance.h"
#include "stone/Compile/LangListener.h"
#include "stone/Compile/Parse.h"
#include "stone/Compile/TypeCheck.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

#include "llvm/IR/Module.h"

using namespace stone;

using stone::LangInstance;
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

  LangInstance lang;
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
  auto &langInvocation = lang.GetLangInvocation();
  auto mainExecPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  langInvocation.SetMainExecutablePath(mainExecPath);

  // Setup the custom formatting to be able to handle syntax diagnostics
  auto diagFormatter = std::make_unique<SyntaxDiagnosticFormatter>();
  auto diagEmitter =
      std::make_unique<TextDiagnosticEmitter>(std::move(diagFormatter));

  TextDiagnosticListener diagListener(std::move(diagEmitter));
  langInvocation.GetContext().GetDiagEngine().AddListener(diagListener);

  auto &ial = langInvocation.ParseArgs(args);
  if (langInvocation.HasError()) {
    return Finish(1);
  }
  auto &mode = langInvocation.ComputeMode(ial);
  if (mode.IsAlien()) {
    // lang.PrintD(SrcLoc(), diags::err_alien_mode)
    Finish(1);
  }
  if (mode.IsPrintHelp()) {
    langInvocation.PrintHelp(langInvocation.GetOpts());
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
  auto inputs = langInvocation.BuildInputFiles(ial);
  if (langInvocation.HasError()) {
    return Finish(1);
  }
  auto sources = langInvocation.BuildSources(inputs);
  if (langInvocation.HasError()) {
    return Finish(1);
  }
  if (lang.GetListener()) {
    lang.GetListener()->OnCompileConfigured(lang);
  }
  lang.Compile(sources);
  if (langInvocation.HasError()) {
    return Finish(1);
  }

  return Finish();
}

class CodeAnalysis final {

  friend LangInstance;
  LangInstance &lang;

public:
  CodeAnalysis(LangInstance &lang);
  ~CodeAnalysis();

private:
  void Analyze(llvm::ArrayRef<SourceUnit *> sources);
  void Analyze(SourceUnit &source);

  SyntaxFile *Parse(const unsigned srcID);

  void ResolveUse();

  /// Print out the syntax tree
  void EmitParse(syn::SyntaxFile *sf);

  /// Perform type-checking on the SyntaxFile
  void TypeCheckSyntaxFile(syn::SyntaxFile &sf);

  /// Perform type-checking on the entire module
  void TypeCheckModule(syn::Module *mod);

  /// Emit the syntax after performing type-checking
  void EmitSyntax(syn::SyntaxFile *sf);
};

CodeAnalysis::CodeAnalysis(LangInstance &lang) : lang(lang) {}
CodeAnalysis::~CodeAnalysis() {}

void CodeAnalysis::Analyze(llvm::ArrayRef<SourceUnit *> sources) {
  for (auto source : sources) {
    assert(source);
    Analyze(*source);
  }
  if (lang.GetLangInvocation().GetMode().JustParse()) {
    return;
  }
  if (lang.GetLangInvocation().GetTypeCheckMode() ==
      types::TypeCheckMode::WholeModule) {
    TypeCheckModule(nullptr /*TODO: get module*/);
  }
  if (lang.GetLangInvocation().GetMode().JustTypeCheck()) {
    // Do some things
    return;
  }
}
void CodeAnalysis::Analyze(SourceUnit &source) {
  auto syntaxFile = Parse(source.GetSrcID());
  assert(syntaxFile);
  // Add to module

  // TODO: May not need this because we are going to add it to the main module.
  // source.SetSyntaxFile(syntaxFile);

  if (lang.GetLangInvocation().GetMode().IsParse()) {
    return;
  }
  if (lang.GetLangInvocation().GetMode().IsEmitParse()) {
    // lang.EmitParse(syntaxFile);
    return;
  }
  if (lang.GetLangInvocation().GetTypeCheckMode() ==
      types::TypeCheckMode::EachFile) {
    TypeCheckSyntaxFile(*syntaxFile);
  }
  if (lang.GetLangInvocation().GetMode().IsTypeCheck()) {
    return;
  }
  if (lang.GetLangInvocation().GetMode().IsEmitSyntax()) {
    // lang.EmitSyntax(*sntaxFile)
  }
}
SyntaxFile *CodeAnalysis::Parse(const unsigned srcID) {
  // TODO: You are not always creating a Library
  auto sf = SyntaxFile::Make(SyntaxFileKind::Library,
                             *lang.GetModuleSystem().GetMainModule(),
                             lang.GetSyntax().GetSyntaxContext(), srcID);

  syn::ParseSyntaxFile(*sf, lang.GetSyntax());
  return sf;
}

void CodeAnalysis::TypeCheckSyntaxFile(SyntaxFile &sf) {
  assert(sf.stage == syn::SyntaxFileStage::AtImports);
  types::TypeCheckSyntaxFile(sf,
                             lang.GetLangInvocation().GetTypeCheckerOptions());
}
/// Perform type-checking on the entire module
void CodeAnalysis::TypeCheckModule(syn::Module *mod) {
  assert(mod && "Null 'syn::Module'");
  for (auto mf : mod->GetFiles()) {
    if (auto sf = llvm::dyn_cast<SyntaxFile>(mf)) {
      TypeCheckSyntaxFile(*sf);
    }
  }
}

void CodeAnalysis::ResolveUse() {}

class CodeGeneration final {

  friend LangInstance;
  LangInstance &lang;

public:
  CodeGeneration(LangInstance &lang);
  ~CodeGeneration();

private:
  void Generate(const CodeAnalysis &codeAnalysis);

  /// Generate the IR for an entire module
  llvm::Module *GenIR(syn::Module &sf, CodeGenContext &cc);

  /// Generate IR a single SyntaxFile
  llvm::Module *GenIR(syn::SyntaxFile &sf, CodeGenContext &cc);

  /// Generate Object file
  void GenObject(unsigned srcID, llvm::Module *mod, CodeGenContext &cc);

  /// Generate Object file
  void GenBitCode();

  /// Generates a 'test.stonem' file
  void GenModule();
};

CodeGeneration::CodeGeneration(LangInstance &lang) : lang(lang) {}

CodeGeneration::~CodeGeneration() {}

void CodeGeneration::Generate(const CodeAnalysis &codeAnalysis) {

  assert(lang.GetLangInvocation().CanCodeGen());

  // We are performing some low leverl code generation
  CodeGenContext cgc(stone::GetLLVMContext(),
                     lang.GetLangInvocation().GetCodeGenOptions());

  // At this point, we much generate IR for all succeeding modes

  // TODO:
  /// Choose what we are going to do
  // auto llvmMod = stone::GenIR(*syntaxFile, cgc);

  // auto mod = stone::GenIR(GetMainModule(), cgc);

  if (lang.GetLangInvocation().GetMode().IsEmitIR()) {
    // EmitIR()
    return;
  }

  if (lang.GetLangInvocation().GetMode().IsNone() ||
      lang.GetLangInvocation().GetMode().IsEmitObject()) {
    // GenObject(srcID, llvmMod, cgc);
    return;
  }
}
llvm::Module *CodeGeneration::GenIR(syn::SyntaxFile &sf, CodeGenContext &cc) {
  return nullptr;
}

llvm::Module *CodeGeneration::GenIR(syn::Module &mod, CodeGenContext &cc) {
  return nullptr;
}

void CodeGeneration::GenObject(const unsigned srcID, llvm::Module *mod,
                               CodeGenContext &cc) {
  /// TODO: This is the only time we should perform a lookup
  // auto outputFile = lang.GetLangInvocation().ComputeOutputFile(srcID);
  // auto result GenObject(cgc GetSyntaxContext(), outputFile.get());
}

class CodeOptimization final {
  friend LangInstance;
  LangInstance &lang;

public:
  CodeOptimization(LangInstance &lang);
  ~CodeOptimization();

  void Optimize();
};

CodeOptimization::CodeOptimization(LangInstance &lang) : lang(lang) {}

CodeOptimization::~CodeOptimization() {}

// Peform code generation
void CodeOptimization::Optimize() {

  if (lang.GetLangInvocation().GetCodeGenOptions().skipOptimization) {
    /// Send the SyntaxFile to the optimizer
    // OptimizeIR(llvmMod);
    return;
  }
}
void LangInstance::Compile(llvm::ArrayRef<SourceUnit *> sources) {
  if (listener) {
    listener->OnCompileStarted(*this);
  }
  // Every case requires code-analysis
  CodeAnalysis codeAnalysis(*this);
  codeAnalysis.Analyze(sources);

  if (langInvocation.JustAnalysis()) {
    // Do some things
    return;
  }
  // At this point, we should have a module with one or more syntax files
  CodeGeneration codeGeneration(*this);
  // Pass codeAnalysis for now -- may just pass module or file
  codeGeneration.Generate(codeAnalysis);
}
