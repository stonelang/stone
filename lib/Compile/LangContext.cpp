#include "stone/Compile/LangContext.h"
#include "stone/Core/CompileDiagnostic.h"
#include "stone/Core/Defer.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Option/Options.h"

using namespace stone;
using namespace stone::opts;

LangContext::LangContext() {}
LangContext::~LangContext() {}

SourceProfile::~SourceProfile() {}

SourceProfile *SourceProfile::Allocate(const unsigned srcID,
                                       const file::File &input,
                                       LangContext &lc) {
  auto sizePtr =
      LangContext::Allocate<SourceProfile>(lc, sizeof(SourceProfile));
  return ::new (sizePtr) SourceProfile(srcID, input);
}

static void ParseCodeGenArguments() {}
static void ParseTypeCheckerArguments() {}
static void ParseSearchPathArguments() {}

bool LangContext::ParseArgs(llvm::ArrayRef<const char *> args) {

  support.SetExcludedFlagsBitmask(opts::NoLangOption);

  if (support.ParseArgs(args, &ctx) == stone::Err) {
    return stone::Err;
  }

  // support.BuildInputFiles(GetInputArgList());

  // for (auto &input : inputs) {
  //   auto sf BuildSourceProfile(input);
  //   sources.
  // }

  /// Now, build the source profiles
  return stone::Ok;
}

unsigned LangContext::CreateSourceBuffer(const file::File &input) {

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

SourceProfile *LangContext::BuildSourceProfile(const file::File &input) {
  auto srcID = CreateSourceBuffer(input);
  auto sp = SourceProfile::Allocate(srcID, input, *this);
  sources.insert(std::make_pair(srcID, sp));
}

std::unique_ptr<OutputFile>
LangContext::ComputeOutputFile(const unsigned srcID) {

  stone::Panic("ComputeSourceOutputFile not implemented");
}