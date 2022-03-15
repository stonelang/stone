#include "stone/Compile/Lang.h"
#include "stone/Compile/LangListener.h"
#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/Defer.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/TextDiagnosticEmitter.h"

#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using stone::FileMgr;
using stone::Lang;
using stone::LangStats;
using stone::ModeKind;
using stone::SrcMgr;
using stone::syn::Identifier;
using stone::syn::Module;
using stone::syn::Syntax;
using stone::syn::SyntaxContext;
using stone::syn::SyntaxFileKind;

using namespace stone;
using namespace stone::syn;

Lang::Lang(LangListener *listener) : listener(listener) {

  stats = std::make_unique<LangStats>(*this, lc.GetContext());

  lc.GetContext().GetStatEngine().Register(stats.get());

  tc = std::make_unique<syn::SyntaxContext>(lc.GetContext(),
                                            lc.GetLangOptions().searchPathOpts);

  syntax = std::make_unique<syn::Syntax>(*tc.get());

  moduleSystem = std::make_unique<syn::ModuleSystem>(*syntax.get());
}
Lang::~Lang() {}

void Lang::Initialize() {}

// // Build the session
// void Lang::BuildSession(const llvm::opt::InputArgList &ial) {

//   llvm::PrettyStackTraceString crashInfo("Lang Initialization");
//   assert(GetSessionKind() == SessionKind::Lang && "Invalid SessionKind");

//   auto &tal = TranslateInputArgList(ial);
//   ComputeModeKind(tal);

//   if (GetMode().Is(ModeKind::Unknown)) {
//     stone::Panic("Unknown mode kind -- this will be handled by diagonstics");
//   }
//   BuildInputFiles(tal);
// }

// void Lang::BuildUnits() {
//   for (auto &input : GetLangOptions().inputFiles) {
//     assert(file::Exists(input.GetName()) && "Input file does not exist.");

//     auto fileBuffer =
//         GetContext().GetFileMgr().getBufferForFile(input.GetName());
//     if (!fileBuffer) {
//       // Printd(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
//       //          diag::LLVMStrArgument(input.GetName()));
//       return;
//     }
//     auto srcID =
//         GetContext().GetSrcMgr().addNewSourceBuffer(std::move(*fileBuffer));
//     assert((srcID > 0) && "Input file buffer ID must be greater than zero.");

//     /// TODO: optimize
//     auto unit = std::make_unique<LangUnit>(input, srcID);
//     units.Add(std::move(unit));
//   }
// }

// ModeKind Lang::GetDefaultMode() { return ModeKind::EmitObject; }
// void Lang::BuildOptions() {}

// llvm::StringRef Lang::GetProgramName() { return name; }
// llvm::StringRef Lang::GetProgramPath() { return path; }

syn::Module *Lang::GetMainModule() const {

  if (mainModule) {
    return mainModule;
  }
  Identifier &moduleName = tc->GetIdentifier(GetModuleName());
  mainModule = syntax->MakeModuleDecl(moduleName, true);
  return mainModule;
}

void Lang::PrintHelp() { lc.GetSupport().PrintHelp(); }

void Lang::PrintVersion() {}

void Lang::Finish() {
  if (listener) {
    listener->OnCompileCompleted(*this);
  }
}

void LangStats::Print() {

  // if (sc.GetLangOpts().printStats) {
  //   // GetContext().Out() << GetName() << '\n';
  //   return;
  // }
}