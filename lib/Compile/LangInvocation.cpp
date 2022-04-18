#include "stone/Compile/LangInvocation.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/LangOptionsConverter.h"
#include "stone/Session/Options.h"

using namespace stone;
using namespace stone::opts;

LangInvocation::LangInvocation() {
  SetExcludedFlagsBitmask(opts::NoLangOption);
}
LangInvocation::~LangInvocation() {}

SourceUnit::~SourceUnit() {}

SourceUnit *SourceUnit::Allocate(const unsigned srcID, const file::File &input,
                                 LangInvocation &langInvocation) {
  auto sizePtr =
      LangInvocation::Allocate<SourceUnit>(langInvocation, sizeof(SourceUnit));
  return ::new (sizePtr) SourceUnit(srcID, input);
}

stone::Error ParseLangArgs(
    llvm::opt::InputArgList &ial,

    LangInvocation &langInvocation,
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  LangOptionsConverter converter(langInvocation.GetContext().GetDiagEngine(),
                                 ial, langInvocation.GetMode(),
                                 langInvocation.GetLangOptions());

  return stone::Error(converter.Convert(buffers));
}

static void ParseSystemArgs(llvm::opt::InputArgList &ial,
                            LangInvocation &langInvocation) {}
static void ParseCodeGenArgs(llvm::opt::InputArgList &ial,
                             LangInvocation &invocation) {}
static void ParseTypeCheckerArgs(llvm::opt::InputArgList &ial,
                                 LangInvocation &invocation) {}
static void ParseSearchPathArgs(llvm::opt::InputArgList &ial,
                                LangInvocation &invocation) {}

llvm::opt::InputArgList &
LangInvocation::ParseArgs(llvm::ArrayRef<const char *> args) {

  auto &ial = Session::ParseArgs(args);
  if (ParseLangArgs(ial, *this, nullptr /* pass null for now*/).Has()) {
    // TODO: return stone::Error();
    stone::Panic("Not implemented");
  }
  ParseSystemArgs(ial, *this);
  ParseTypeCheckerArgs(ial, *this);
  ParseSearchPathArgs(ial, *this);
  ParseCodeGenArgs(ial, *this);

  return ial;
}

llvm::ArrayRef<SourceUnit *>
LangInvocation::BuildSources(const file::Files &inputs) {
  for (auto &input : inputs) {
    auto source = BuildSource(input);
    assert(source);
    sources.push_back(source);
  }
  return sources;
}

SourceUnit *LangInvocation::BuildSource(const file::File &input) {
  auto srcID = CreateSourceID(input);
  return SourceUnit::Allocate(srcID, input, *this);
}
unsigned LangInvocation::CreateSourceID(const file::File &input) {
  auto fb = ctx.GetFileMgr().getBufferForFile(input.GetName());
  if (!fb) {
    ctx.PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
               diag::LLVMStr(input.GetName()));
  }
  auto srcID = ctx.GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
  return srcID;
}

void LangInvocation::RecordPrimarySourceID(unsigned primarySourceID) {
  stone::Panic("RecordPrimarySourceID not implemented");
}

std::unique_ptr<OutputFile>
LangInvocation::ComputeOutputFile(SourceUnit &source) {
  stone::Panic("ComputeSourceOutputFile not implemented");
}