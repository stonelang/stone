#include "stone/Compile/Frontend.h"
#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/Defer.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Option/Options.h"

using namespace stone;
using namespace stone::opts;

Frontend::Frontend() { SetExcludedFlagsBitmask(opts::NoLangOption); }
Frontend::~Frontend() {}

SourceUnit::~SourceUnit() {}

SourceUnit *SourceUnit::Allocate(const unsigned srcID, const file::File &input,
                                 Frontend &frontend) {

  auto sizePtr = Frontend::Allocate<SourceUnit>(frontend, sizeof(SourceUnit));
  return ::new (sizePtr) SourceUnit(srcID, input);
}

static void ParseCodeGenArguments() {}
static void ParseTypeCheckerArguments() {}
static void ParseSearchPathArguments() {}

// llvm::opts::InputArgList& Frontend::ParseArgs(llvm::ArrayRef<const char *>
// args) {
// auto result = OptBase::ParseArgs(....)
// }

unsigned Frontend::CreateSourceID(const file::File &input) {

  auto fb = ctx.GetFileMgr().getBufferForFile(input.GetName());
  if (!fb) {
    // ctx.Printd(SrcLoc(),
    // diag::err_unable_to_open_buffer_for_file,
    //          diag::LLVMStrArgument(input.GetName()));
    stone::Panic("err_unable_to_open_buffer_for_file");
  }
  auto srcID = ctx.GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
  return srcID;
}

llvm::ArrayRef<unsigned> Frontend::CreateSourceIDs(file::Files &inputs) {
  for (auto &input : inputs) {
    auto sourceID = CreateSourceID(input);
    sourceIDs.push_back(sourceID);
  }
  return sourceIDs;
}

// SourceUnit *Frontend::BuildSourceUnit(const file::File &input) {
//   auto srcID = CreateSourceBuffer(input);
//   auto sp = SourceUnit::Allocate(srcID, input, *this);
//   sources.insert(std::make_pair(srcID, sp));
// }

std::unique_ptr<OutputFile> Frontend::ComputeOutputFile(const unsigned srcID) {
  stone::Panic("ComputeSourceOutputFile not implemented");
}