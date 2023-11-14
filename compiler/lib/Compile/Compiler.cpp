#include "stone/Compile/Compiler.h"
#include "stone/Basic/Mem.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Public.h"

#include "clang/Basic/FileManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

using namespace stone;
using namespace stone::syn;

// Compiler::Compiler() : queue(*this), stats(new CompilerStats(*this)) {
//   GetCompilerContext().GetLangContext().GetStats().Register(stats.get());
// }

Compiler::Compiler() = default;
Compiler::~Compiler() = default;

Status Compiler::Configure() {
  compilerStats.reset(new CompilerStats(*this));
  compilerQueue.reset(new CompilerQueue(*this));

  return Status();
}

// void Compiler::Setup() {

//   // compilerStats.reset(new CompilerStats(*this));
//   //
//   GetCompilerContext().GetLangContext().GetStats().Register(compilerStats);
//   // SetupSyntaxContext();
// }

/// Setup SyntaxContext based on the action
// void Compiler::SetupSyntaxContext() {

//   syntaxContext.reset(new
//   SyntaxContext(GetCompilerContext().GetLangContext(),
//                                         GetCompilerContext().GetSearchPathOptions(),
//                                         GetCompilerContext().GetClangContext()));
// }

// std::unique_ptr<llvm::raw_fd_ostream>
// Compiler::GetFileOutputStream(llvm::StringRef outputFilename, LangContext
// &lc) {
//   std::error_code errCode;
//   auto os = std::make_unique<llvm::raw_fd_ostream>(outputFilename, errCode,
//                                                    llvm::sys::fs::OF_None);
//   if (errCode) {
//     lc.GetDiags().PrintD(SrcLoc(), diag::err_opening_output,
//                          diag::LLVMStr(outputFilename),
//                          diag::LLVMStr(errCode.message()));
//     return nullptr;
//   }
//   return os;
// }

// void Compiler::FinishTypeCheck() {
// }

// llvm::StringRef Compiler::ComputeSourceOutputFile(unsigned srcID) {
//   assert(false && "Not implemented");
//   return llvm::StringRef();
// }

const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const {
  return GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForAtMostOnePrimary() const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForAtMostOnePrimary();
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForPrimary(StringRef filename) const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(filename);
}
const PrimaryFileSpecificPaths &
Compiler::GetPrimaryFileSpecificPathsForSyntaxFile(
    const syn::SyntaxFile &sf) const {
  return GetCompilerOptions()
      .GetInputsAndOutputs()
      .GetPrimaryFileSpecificPathsForPrimary(sf.GetFilename());
}

// void Compiler::ResolveImports() {
//   // Resolve imports for all the source files.
//   for (auto *moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//     if (auto *syntaxFile = dyn_cast<syn::SyntaxFile>(moduleFile))
//       stone::ResolveSyntaxFileImports(*syntaxFile);
//   }
// }

// Status Compiler::ForEachSyntaxFileToTypeCheck(
//     EachSyntaxFileToTypeCheckCallback notify) {

// if (GetCompilerContext().GetTypeCheckMode() == TypeCheckMode::WholeModule) {

//   for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//     auto *syntaxFile = dyn_cast<syn::SyntaxFile>(moduleFile);
//     if (!syntaxFile) {
//       continue;
//     }
//     if (notify(*syntaxFile, GetCompilerContext().GetTypeCheckerOptions(),
//                GetListener())
//             .IsError()) {
//       return Status::Error();
//     }
//   }
// } else {
//   for (auto *syntaxFile :
//        GetModuleSystem().GetMainModule()->GetPrimarySyntaxFiles()) {
//     if (notify(*syntaxFile, GetCompilerContext().GetTypeCheckerOptions(),
//                GetListener())
//             .IsError()) {
//       return Status::Error();
//     }
//   }
// }

//   return Status();
// }

// Status Compiler::ForEachSyntaxFile(EachSyntaxFileCallback notify) {

// for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//   auto *syntaxFile = dyn_cast<SyntaxFile>(moduleFile);
//   if (!syntaxFile) {
//     continue;
//   }
//   if (notify(*syntaxFile).IsError()) {
//     return Status::Error();
//   }
// }
// return Status();
//}

void Compiler::AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
  diags.AddConsumer(consumer);
}

// size_t Compiler::GetTotalMemUsed() const { return bumpAlloc.getTotalMemory();
// }

// void Compiler::PrintTimers() {}
// void Compiler::PrintDiagnostics() {}
// void Compiler::PrintStatistics() {}

void *stone::AllocateInCompiler(size_t bytes, const Compiler &compiler,
                                mem::AllocationArena arena,
                                unsigned alignment) {
  return nullptr;
}

void Compiler::Finish() {}

CompilerAction::CompilerAction() {}

// Status Compiler::CreateSourceBuffers() {

//   // Adds to InputSourceCodeBufferIDs, so may need to happen before the
//   // per-input setup.
//   const llvm::Optional<unsigned> codeCompletionBufferID =
//       CreateCodeCompletionBuffer();

//   const auto &inputs = GetCompilerOptions().inputsAndOutputs.GetInputs();
//   const bool shouldRecover =
//       GetCompilerOptions().inputsAndOutputs.ShouldRecoverMissingInputs();

//   bool hasFailed = false;
//   for (const CompilerInputFile &input : inputs) {
//     bool failed = false;
//     llvm::Optional<unsigned> bufferID =
//         GetRecordedBufferID(input, shouldRecover, failed);
//     hasFailed |= failed;

//     if (!bufferID.hasValue() || !input.IsPrimary()) {
//       continue;
//     }
//     RecordPrimarySourceID(*bufferID);
//   }
//   if (hasFailed) {
//     return Status::Error();
//   }

//   return Status();
// }

// llvm::Optional<unsigned> Compiler::GetRecordedBufferID(
//     const CompilerInputFile &input, const bool shouldRecover, bool &failed) {
//   if (!input.GetBuffer()) {
//     if (llvm::Optional<unsigned> existingBufferID =
//             GetLangContext().GetSrcMgr().getIDForBufferIdentifier(
//                 input.GetFileName())) {
//       return existingBufferID;
//     }
//   }
//   auto buffers = GetInputBuffersIfPresent(input);

//   // Recover by dummy buffer if requested.
//   if (!buffers.hasValue() && shouldRecover &&
//       input.GetType() == file::Type::Stone) {
//     buffers = ModuleBuffers(llvm::MemoryBuffer::getMemBuffer(
//         "// missing file\n", input.GetFileName()));
//   }

//   if (!buffers.hasValue()) {
//     failed = true;
//     return llvm::None;
//   }

// FIXME: The fact that this test happens twice, for some cases,
// suggests that setupInputs could use another round of refactoring.
// TODO:
// if (serialization::isSerializedAST(buffers->ModuleBuffer->getBuffer())){
//   PartialModules.push_back(std::move(*buffers));
//   return None;
// }

// TODO
// assert(buffers->moduleDocBuffer.get() == nullptr);
// assert(buffers->moduleSourceInfoBuffer.get() == nullptr);

// Transfer ownership of the MemoryBuffer to the SourceMgr.
//   unsigned bufferID = GetLangContext().GetSrcMgr().addNewSourceBuffer(
//       std::move(buffers->moduleBuffer));

//   sourceBufferIDs.push_back(bufferID);
//   return bufferID;
// }

// // TODO:
// llvm::Optional<ModuleBuffers>
// Compiler::GetInputBuffersIfPresent(
//     const CompilerInputFile &input) {

//   // if (auto b = input.GetBuffer()) {
//   //   return ModuleBuffers(llvm::MemoryBuffer::getMemBufferCopy(
//   //       b->getBuffer(), b->getBufferIdentifier()));
//   // }

//   // // FIXME: Working with filenames is fragile, maybe use the real path
//   // // or have some kind of FileManager.

//   // using InputFileOrError =
//   // llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>; InputFileOrError
//   // inputFileOrError =
//   //     GetLangContext().GetFileMgr().getBufferForFile(input.GetFileName());

//   // if (!inputFileOrError) {
//   //   GetLangContext().GetDiags().PrintD(SrcLoc(),
//   // diag::err_unable_to_open_buffer_for_file,
//   // diag::LLVMStr(input.GetFileName()));
//   //   return llvm::None;
//   // }

//   // // Just return the file buffer for now
//   // return ModuleBuffers(std::move(*inputFileOrError));
//   // if (!fb) {
//   //   GetLangContext().GetDiags().PrintD(SrcLoc(),
//   //   diag::err_unable_to_open_buffer_for_file,
//   //                            diag::LLVMStr(input.GetFileName()));
//   // }
//   // auto srcID =
//   // GetLangContext().GetSrcMgr().addNewSourceBuffer(std::move(*fb));
//   // assert((srcID > 0) && "Input file buffer ID must be greater thanzero.");
//   // return srcID;

//   // FileOrError inputFileOrErr =
//   //   swift::vfs::getFileOrSTDIN(getFileSystem(), input.getFileName(),
//   //                             /*FileSize*/-1,
//   //                             /*RequiresNullTerminator*/true,
//   //                             /*IsVolatile*/false,
//   //     /*Bad File Descriptor Retry*/getInvocation().getCompilerOptions()
//   //                              .BadFileDescriptorRetryCount);
//   // if (!inputFileOrErr) {
//   //   Diagnostics.diagnose(SourceLoc(), diag::error_open_input_file,
//   //                        input.getFileName(),
//   //                        inputFileOrErr.getError().message());
//   //   return None;
//   // }
//   // if (!serialization::isSerializedAST((*inputFileOrErr)->getBuffer()))
//   //   return ModuleBuffers(std::move(*inputFileOrErr));

//   // auto swiftdoc = openModuleDoc(input);
//   // auto sourceinfo = openModuleSourceInfo(input);
//   // return ModuleBuffers(std::move(*inputFileOrErr),
//   //                      swiftdoc.hasValue() ?
//   std::move(swiftdoc.getValue()) :
//   //                      nullptr, sourceinfo.hasValue() ?
//   //                      std::move(sourceinfo.getValue()) : nullptr);

//   return llvm::None;
// }

void CompilerPrettyStackTrace::print(llvm::raw_ostream &os) const {

  //   auto effective =
  //   GetCompilerContext().GetCompilerOptions().effectiveCompilerVersion; if
  //   (effective
  //   != version::Version::GetCurrentCompilerVersion()) {
  //     os << "Compiling with effective version " << effective;
  //   } else {
  //     os << "Compiling with the current compilerInvocationuage version";
  //   }
  //   if
  //   (Invocation.GetCompilerContext().GetCompilerOptions().allowModuleWithCompilerErrors)
  //   {
  //     os << " while allowing modules with compiler errors";
  //   }
  //   os << "\n";
}
void CompilerStats::Print(ColorStream &stream) {
  // if (sc.GetCompilerOpts().printStats) {
  //   // GetLangContext().Out() << GetName() << '\n';
  //   return;
  // }
}
