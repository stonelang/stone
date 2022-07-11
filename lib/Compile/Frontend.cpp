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

FrontendBase::FrontendBase(llvm::StringRef programName,
                           llvm::StringRef programPath)
    : Session(programName, programPath) {
  excludedFlagsBitmask = opts::NoFrontendOption;
}
FrontendBase::~FrontendBase() {}

llvm::Optional<unsigned> FrontendBase::CreateCodeCompletionBuffer() {
  llvm::Optional<unsigned> codeCompletionBufferID;
  // auto codeCompletePoint = GetCodeCompletionPoint();
  // if (codeCompletePoint.first) {
  //   auto memBuf = codeCompletePoint.first;
  //   // CompilerInvocation doesn't own the buffers, copy to a new buffer.
  //   codeCompletionBufferID = SourceMgr.addMemBufferCopy(memBuf);
  //   InputSourceCodeBufferIDs.push_back(*codeCompletionBufferID);
  //   SourceMgr.setCodeCompletionPoint(*codeCompletionBufferID,
  //                                    codeCompletePoint.second);
  // }
  return codeCompletionBufferID;
}

Error FrontendBase::CreateSourceBuffers() {

  // Adds to InputSourceCodeBufferIDs, so may need to happen before the
  // per-input setup.
  const llvm::Optional<unsigned> codeCompletionBufferID =
      CreateCodeCompletionBuffer();

  const auto &inputs = GetFrontendOptions().inputsAndOutputs.GetInputs();

  const bool shouldRecover =
      GetFrontendOptions().inputsAndOutputs.ShouldRecoverMissingInputs();

  bool hasFailed = false;
  for (const FrontendInputFile &input : inputs) {
    bool failed = false;
    llvm::Optional<unsigned> bufferID =
        GetRecordedBufferID(input, shouldRecover, failed);
    hasFailed |= failed;

    if (!bufferID.hasValue() || !input.isPrimary()) {
      continue;
    }
    RecordPrimaryInputBuffer(*bufferID);
  }
  if (hasFailed) {
    return true;
  }
}

Optional<unsigned>
FrontendBase::GetRecordedBufferID(const FrontendInputFile &input,
                                  const bool shouldRecover, bool &failed) {
  if (!input.GetBuffer()) {
    if (llvm::Optional<unsigned> existingBufferID =
            ctx.GetSrcMgr().getIDForBufferIdentifier(input.GetFileName())) {
      return existingBufferID;
    }
  }
  auto buffers = GetInputBuffersIfPresent(input);

  // Recover by dummy buffer if requested.
  if (!buffers.hasValue() && shouldRecover &&
      input.getType() == file::Type::Stone) {
    buffers = ModuleBuffers(llvm::MemoryBuffer::getMemBuffer(
        "// missing file\n", input.getFileName()));
  }

  if (!buffers.hasValue()) {
    failed = true;
    return None;
  }

  // FIXME: The fact that this test happens twice, for some cases,
  // suggests that setupInputs could use another round of refactoring.
  if (serialization::isSerializedAST(buffers->ModuleBuffer->getBuffer())) {
    PartialModules.push_back(std::move(*buffers));
    return None;
  }
  assert(buffers->ModuleDocBuffer.get() == nullptr);
  assert(buffers->ModuleSourceInfoBuffer.get() == nullptr);
  // Transfer ownership of the MemoryBuffer to the SourceMgr.
  unsigned bufferID =
      SourceMgr.addNewSourceBuffer(std::move(buffers->ModuleBuffer));

  sourceBufferIDs.push_back(bufferID);
  return bufferID;
}
unsigned FrontendBase::CreateSourceBuffer(const file::File &input) {
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
  primarySourceBufferIDs.insert(primarySourceID);
}

// std::unique_ptr<OutputFile>
// FrontendBase::ComputeOutputFile(FrontendUnit &source) {
//   stone::Panic("ComputeSourceOutputFile not implemented");
// }

Frontend::Frontend(llvm::StringRef programName, llvm::StringRef programPath,
                   FrontendListener *listener)
    : FrontendBase(programName, programPath), listener(listener) {
  stats = std::make_unique<FrontendStats>(*this);

  GetContext().GetStatEngine().Register(stats.get());
  auto syntaxContext =
      std::make_unique<syn::SyntaxContext>(GetContext(), searchPathOpts);

  syntax = std::make_unique<syn::Syntax>(std::move(syntaxContext));
  moduleSystem = std::make_unique<ModuleSystem>(*syntax.get(), GetContext());
}
Frontend::~Frontend() {}

std::unique_ptr<llvm::raw_fd_ostream>
Frontend::GetFileOutputStream(llvm::StringRef outputFilename, Context &ctx) {
  std::error_code errCode;
  auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, errCode,
                                                   llvm::sys::fs::OF_None);
  if (errCode) {
    ctx.GetDiagUnit().PrintD(SrcLoc(), diag::err_opening_output,
                             diag::LLVMStr(outputFilename),
                             diag::LLVMStr(errCode.message()));
    return nullptr;
  }
  return os;
}
// void Frontend::FinishTypeCheck() {
// }

// llvm::StringRef Frontend::ComputeSourceOutputFile(unsigned srcID) {
//   assert(false && "Not implemented");
//   return llvm::StringRef();
// }


void Frontend::PrintHelp(const llvm::opt::OptTable &opts) {}

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