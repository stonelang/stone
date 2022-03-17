#include "stone/Compile/Frontend.h"
#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/Defer.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Option/Options.h"

using namespace stone;
using namespace stone::opts;

Frontend::Frontend() : optUtil(GetLangOptions()) {}
Frontend::~Frontend() {}

SourceUnit::~SourceUnit() {}

SourceUnit *SourceUnit::Allocate(const unsigned srcID, const file::File &input,
                                 Frontend &lc) {
  auto sizePtr = Frontend::Allocate<SourceUnit>(lc, sizeof(SourceUnit));
  return ::new (sizePtr) SourceUnit(srcID, input);
}

static void ParseCodeGenArguments() {}
static void ParseTypeCheckerArguments() {}
static void ParseSearchPathArguments() {}

bool Frontend::ParseArgs(llvm::ArrayRef<const char *> args) {

  GetOptUtil().SetExcludedFlagsBitmask(opts::NoLangOption);

  if (GetOptUtil().ParseArgs(args, &ctx) == stone::Err) {
    return stone::Err;
  }

  for (auto &input : langOpts.inputFiles) {
    BuildSourceUnit(input);
  }
  /// Now, build the source profiles
  return stone::Ok;
}

unsigned Frontend::CreateSourceBuffer(const file::File &input) {

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

SourceUnit *Frontend::BuildSourceUnit(const file::File &input) {
  auto srcID = CreateSourceBuffer(input);
  auto sp = SourceUnit::Allocate(srcID, input, *this);
  sources.insert(std::make_pair(srcID, sp));
}

std::unique_ptr<OutputFile>
Frontend::ComputeOutputFile(const unsigned srcID) {

  stone::Panic("ComputeSourceOutputFile not implemented");
}