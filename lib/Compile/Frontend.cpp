#include "stone/Compile/Frontend.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/FrontendListener.h"
#include "stone/Diag/FrontendDiagnostic.h"

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
using namespace stone::opts;

FrontendBase::FrontendBase() {
  SetExcludedFlagsBitmask(opts::NoFrontendOption);
}
FrontendBase::~FrontendBase() {}

FrontendUnit::~FrontendUnit() {}

FrontendUnit *FrontendUnit::Allocate(const unsigned srcID,
                                     const file::File &input,
                                     Frontend &frontend) {
  auto sizePtr =
      Frontend::Allocate<FrontendUnit>(frontend, sizeof(FrontendUnit));
  return ::new (sizePtr) FrontendUnit(srcID, input);
}

llvm::ArrayRef<FrontendUnit *>
FrontendBase::BuildSources(const file::Files &inputs) {
  for (auto &input : inputs) {
    auto source = BuildSource(input);
    assert(source);
    sources.push_back(source);
  }
  return sources;
}

FrontendUnit *FrontendBase::BuildSource(const file::File &input) {
  auto srcID = CreateSourceID(input);
  return FrontendUnit::Allocate(srcID, input, static_cast<Frontend &>(*this));
}
unsigned FrontendBase::CreateSourceID(const file::File &input) {
  auto fb = ctx.GetFileMgr().getBufferForFile(input.GetName());
  if (!fb) {
    ctx.GetDiagUnit().PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
                             diag::LLVMStr(input.GetName()));
  }
  auto srcID = ctx.GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
  return srcID;
}

void FrontendBase::RecordPrimarySourceID(unsigned primarySourceID) {
  stone::Panic("RecordPrimarySourceID not implemented");
}

std::unique_ptr<OutputFile>
FrontendBase::ComputeOutputFile(FrontendUnit &source) {
  stone::Panic("ComputeSourceOutputFile not implemented");
}

Frontend::Frontend(FrontendListener *listener) : listener(listener) {
  stats = std::make_unique<FrontendStats>(*this);

  GetContext().GetStatEngine().Register(stats.get());

  auto syntaxContext =
      std::make_unique<syn::SyntaxContext>(GetContext(), searchPathOpts);

  syntax = std::make_unique<syn::Syntax>(std::move(syntaxContext));

  moduleSystem = std::make_unique<ModuleSystem>(*syntax.get(), GetContext());
}
Frontend::~Frontend() {}

void Frontend::Initialize() {}

std::unique_ptr<llvm::raw_fd_ostream>
Frontend::GetFileOutputStream(llvm::StringRef outputFilename, Context &ctx) {
  std::error_code ec;
  auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, ec,
                                                   llvm::sys::fs::OF_None);
  if (ec) {
    ctx.GetDiagUnit().PrintD(SrcLoc(), diag::err_opening_output,
                             diag::LLVMStr(outputFilename),
                             diag::LLVMStr(ec.message()));
    return nullptr;
  }
  return os;
}

// llvm::StringRef Frontend::GetProgramName() { return name; }
// llvm::StringRef Frontend::GetProgramPath() { return path; }

// void Frontend::RecordPrimaryInputBuffer(unsigned bufferID) {
//   PrimaryBufferIDs.insert(bufferID);
// }
// void Frontend::FinishTypeCheck() {
// }

llvm::StringRef Frontend::ComputeSourceOutputFile(unsigned srcID) {
  assert(false && "Not implemented");
  return llvm::StringRef();
}

void Frontend::PrintVersion() {}

void Frontend::PrintHelp() {}
void Frontend::Finish() {
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