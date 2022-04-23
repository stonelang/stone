#include "stone/Compile/FrontendInstance.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/FrontendListener.h"
#include "stone/Parse/Parse.h"
#include "stone/Sem/TypeCheck.h"
#include "stone/Sem/UsingResolution.h"

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

FrontendInstance::FrontendInstance(FrontendListener *listener)
    : listener(listener) {
  stats = std::make_unique<FrontendStats>(*this);

  langInvocation.GetContext().GetStatEngine().Register(stats.get());

  auto syntaxContext = std::make_unique<syn::SyntaxContext>(
      langInvocation.GetContext(),
      langInvocation.GetFrontendOptions().searchPathOpts);

  syntax = std::make_unique<syn::Syntax>(std::move(syntaxContext));

  moduleSystem = std::make_unique<ModuleSystem>(
      *syntax.get(), langInvocation.GetFrontendOptions());
}
FrontendInstance::~FrontendInstance() {}

void FrontendInstance::Initialize() {}

std::unique_ptr<llvm::raw_fd_ostream>
FrontendInstance::GetFileOutputStream(llvm::StringRef outputFilename,
                                      Context &ctx) {
  std::error_code ec;
  auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, ec,
                                                   llvm::sys::fs::OF_None);
  if (ec) {
    ctx.PrintD(SrcLoc(), diag::err_opening_output,
               diag::LLVMStr(outputFilename), diag::LLVMStr(ec.message()));
    return nullptr;
  }
  return os;
}
// // Build the session
// void Frontend::BuildSession(const llvm::opt::InputArgList &ial) {

//   llvm::PrettyStackTraceString crashInfo("Frontend Initialization");
//   assert(GetSessionKind() == SessionKind::Frontend && "Invalid SessionKind");

//   auto &tal = TranslateInputArgList(ial);
//   ComputeModeKind(tal);

//   if (GetMode().Is(ModeKind::Unknown)) {
//     stone::Panic("Unknown mode kind -- this will be handled by diagonstics");
//   }
//   BuildInputFiles(tal);
// }

// void Frontend::BuildUnits() {
//   for (auto &input : GetFrontendOptions().inputFiles) {
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
//     auto unit = std::make_unique<FrontendUnit>(input, srcID);
//     units.Add(std::move(unit));
//   }
// }

// ModeKind Frontend::GetDefaultMode() { return ModeKind::EmitObject; }
// void Frontend::BuildOptions() {}

// llvm::StringRef Frontend::GetProgramName() { return name; }
// llvm::StringRef Frontend::GetProgramPath() { return path; }

// void FrontendInstance::RecordPrimaryInputBuffer(unsigned bufferID) {
//   PrimaryBufferIDs.insert(bufferID);
// }

// void FrontendInstance::FinishTypeCheck() {
// }

void FrontendInstance::ForEachSyntaxFile(EachSyntaxFileCallback client) {

  switch (GetFrontendInvocation().GetTypeCheckMode()) {
  case TypeCheckMode::WholeModule: {
    for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
      auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile);
      if (syntaxFile) {
        client(*syntaxFile, GetFrontendInvocation().GetTypeCheckerOptions(),
               GetListener());
      }
    }
  }
  case TypeCheckMode::EachFile: {
    for (auto *syntaxFile :
         GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
      client(*syntaxFile, GetFrontendInvocation().GetTypeCheckerOptions(),
             GetListener());
    }
  }
  }
}

llvm::StringRef FrontendInstance::ComputeSourceOutputFile(unsigned srcID) {
  assert(false && "Not implemented");
  return llvm::StringRef();
}

void FrontendInstance::CompileWithSyntaxAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources) {
  CompileWithSyntaxAnalysis(sources, [&](syn::SyntaxFile &sf) {
    return [&](syn::SyntaxFile &sf) -> void {}(sf);
  });
}

void FrontendInstance::CompileWithSyntaxAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources, SyntaxAnalysisCallback client) {

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

  if (!GetFrontendInvocation().GetMode().JustParse()) {
    ResolveUsings();
  }
  if (listener) {
    listener->OnSyntaxAnalysisCompleted(*this);
  }
}

void FrontendInstance::ResolveUsings() {
  // Resolve imports for all the source files.
  for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile))
      sem::ResolveUsings(*syntaxFile);
  }
}
void FrontendInstance::CompileWithSemanticAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources) {

  CompileWithSyntaxAnalysis(sources);

  ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
                        sem::TypeCheckerOptions &typeCheckerOpts,
                        stone::TypeCheckerListener *listener) {
    sem::TypeCheck(syntaxFile, typeCheckerOpts, listener);
  });

  // FinishTypeCheck();
  if (listener) {
    listener->OnSemanticAnalysisCompleted(*this);
  }
}

void FrontendInstance::CompileWithSemanticAnalysis(
    llvm::ArrayRef<FrontendUnit *> &sources, SemanticAnalysisCallback client) {
  CompileWithSemanticAnalysis(sources);
  client(*this);
}

void FrontendInstance::PrintVersion() {}

void FrontendInstance::Finish() {
  if (listener) {
    listener->OnCompileCompleted(*this);
  }
}

void FrontendStats::Print(ColorfulStream &stream) {
  // if (sc.GetFrontendOpts().printStats) {
  //   // GetContext().Out() << GetName() << '\n';
  //   return;
  // }
}