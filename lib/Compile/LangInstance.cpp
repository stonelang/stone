#include "stone/Compile/LangInstance.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/LangListener.h"
#include "stone/Compile/Parse.h"
#include "stone/Compile/TypeCheck.h"
#include "stone/Compile/UsingResolution.h"

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

using namespace stone;
using namespace stone::syn;

LangInstance::LangInstance(LangListener *listener) : listener(listener) {
  stats = std::make_unique<LangStats>(*this);

  langInvocation.GetContext().GetStatEngine().Register(stats.get());

  auto syntaxContext = std::make_unique<syn::SyntaxContext>(
      langInvocation.GetContext(),
      langInvocation.GetLangOptions().searchPathOpts);

  syntax = std::make_unique<syn::Syntax>(std::move(syntaxContext));

  moduleSystem = std::make_unique<ModuleSystem>(
      *syntax.get(), langInvocation.GetLangOptions());
}
LangInstance::~LangInstance() {}

void LangInstance::Initialize() {}

std::unique_ptr<llvm::raw_fd_ostream>
LangInstance::GetFileOutputStream(llvm::StringRef outputFilename,
                                  Context &ctx) {
  std::error_code ec;
  auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, ec,
                                                   llvm::sys::fs::OF_None);
  if (ec) {
    ctx.PrintD(SrcLoc(), diag::error_opening_output,
               diag::LLVMStr(outputFilename), diag::LLVMStr(ec.message()));
    return nullptr;
  }
  return os;
}
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
//       // PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
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

// void LangInstance::RecordPrimaryInputBuffer(unsigned bufferID) {
//   PrimaryBufferIDs.insert(bufferID);
// }

// void LangInstance::FinishTypeCheck() {
// }

void LangInstance::ForEachSyntaxFile(EachSyntaxFileCallback client) {

  switch (GetLangInvocation().GetTypeCheckMode()) {
  case TypeCheckMode::WholeModule: {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile);
      if (syntaxFile) {
        client(*syntaxFile, GetLangInvocation().GetTypeCheckerOptions(),
               GetListener());
      }
    }
  }
  case TypeCheckMode::EachFile: {
    for (auto *syntaxFile :
         GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
      client(*syntaxFile, GetLangInvocation().GetTypeCheckerOptions(),
             GetListener());
    }
  }
  }
}

llvm::StringRef LangInstance::ComputeSourceOutputFile(unsigned srcID) {
  assert(false && "Not implemented");
  return llvm::StringRef();
}

void LangInstance::CompileWithSyntaxAnalysis(
    llvm::ArrayRef<SourceUnit *> &sources) {
  CompileWithSyntaxAnalysis(sources, [&](syn::SyntaxFile &sf) {
    return [&](syn::SyntaxFile &sf) -> void {}(sf);
  });
}

void LangInstance::CompileWithSyntaxAnalysis(
    llvm::ArrayRef<SourceUnit *> &sources, SyntaxAnalysisCallback client) {

  for (auto source : sources) {
    assert(source);
    // TODO: You are not always creating a Library
    auto syntaxFile = SyntaxFile::Make(
        SyntaxFileKind::Library, *GetModuleSystem().GetMainModule(),
        GetSyntax().GetSyntaxContext(), source->GetSrcID());

    syn::Parse(*syntaxFile, GetSyntax(), GetListener());

    assert(syntaxFile);
    client(*syntaxFile);
  }

  if (!GetLangInvocation().GetMode().JustParse()) {
    ResolveUsings();
  }
  if (listener) {
    listener->OnSyntaxAnalysisCompleted(*this);
  }
}

void LangInstance::ResolveUsings() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile))
      types::ResolveUsings(*syntaxFile);
  }
}
void LangInstance::CompileWithSemanticAnalysis(
    llvm::ArrayRef<SourceUnit *> &sources) {

  CompileWithSyntaxAnalysis(sources);

  ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
                        types::TypeCheckerOptions &typeCheckerOpts,
                        stone::TypeCheckerListener *listener) {
    types::TypeCheck(syntaxFile, typeCheckerOpts, listener);
  });

  // FinishTypeCheck();
  if (listener) {
    listener->OnSemanticAnalysisCompleted(*this);
  }
}

void LangInstance::CompileWithSemanticAnalysis(
    llvm::ArrayRef<SourceUnit *> &sources, SemanticAnalysisCallback client) {
  CompileWithSemanticAnalysis(sources);
  client(*this);
}

void LangInstance::PrintVersion() {}

void LangInstance::Finish() {
  if (listener) {
    listener->OnCompileCompleted(*this);
  }
}

void LangStats::Print(ColorfulStream &stream) {
  // if (sc.GetLangOpts().printStats) {
  //   // GetContext().Out() << GetName() << '\n';
  //   return;
  // }
}