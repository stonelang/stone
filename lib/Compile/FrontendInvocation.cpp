#include "stone/Compile/FrontendInvocation.h"
#include "stone/Basic/FrontendDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/FrontendOptionsConverter.h"
#include "stone/Session/Options.h"

using namespace stone;
using namespace stone::opts;

FrontendInvocation::FrontendInvocation() {
  SetExcludedFlagsBitmask(opts::NoFrontendOption);
}
FrontendInvocation::~FrontendInvocation() {}

FrontendUnit::~FrontendUnit() {}

FrontendUnit *FrontendUnit::Allocate(const unsigned srcID,
                                     const file::File &input,
                                     FrontendInvocation &frontendInvocation) {
  auto sizePtr = FrontendInvocation::Allocate<FrontendUnit>(
      frontendInvocation, sizeof(FrontendUnit));
  return ::new (sizePtr) FrontendUnit(srcID, input);
}

stone::Error ParseFrontendArgs(
    llvm::opt::InputArgList &ial,

    FrontendInvocation &frontendInvocation,
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  FrontendOptionsConverter converter(
      frontendInvocation.GetContext().GetDiagEngine(), ial,
      frontendInvocation.GetMode(), frontendInvocation.GetFrontendOptions());

  return stone::Error(converter.Convert(buffers));
}

static void ParseSystemArgs(llvm::opt::InputArgList &ial,
                            FrontendInvocation &frontendInvocation) {}
static void ParseCodeGenArgs(llvm::opt::InputArgList &ial,
                             FrontendInvocation &invocation) {}
static void ParseTypeCheckerArgs(llvm::opt::InputArgList &ial,
                                 FrontendInvocation &invocation) {}
static void ParseSearchPathArgs(llvm::opt::InputArgList &ial,
                                FrontendInvocation &invocation) {}

llvm::opt::InputArgList &
FrontendInvocation::ParseArgs(llvm::ArrayRef<const char *> args) {

  auto &ial = Session::ParseArgs(args);
  if (ParseFrontendArgs(ial, *this, nullptr /* pass null for now*/).Has()) {
    // TODO: return stone::Error();
    stone::Panic("Not implemented");
  }
  ParseSystemArgs(ial, *this);
  ParseTypeCheckerArgs(ial, *this);
  ParseSearchPathArgs(ial, *this);
  ParseCodeGenArgs(ial, *this);

  return ial;
}

llvm::ArrayRef<FrontendUnit *>
FrontendInvocation::BuildSources(const file::Files &inputs) {
  for (auto &input : inputs) {
    auto source = BuildSource(input);
    assert(source);
    sources.push_back(source);
  }
  return sources;
}

FrontendUnit *FrontendInvocation::BuildSource(const file::File &input) {
  auto srcID = CreateSourceID(input);
  return FrontendUnit::Allocate(srcID, input, *this);
}
unsigned FrontendInvocation::CreateSourceID(const file::File &input) {
  auto fb = ctx.GetFileMgr().getBufferForFile(input.GetName());
  if (!fb) {
    ctx.PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
               diag::LLVMStr(input.GetName()));
  }
  auto srcID = ctx.GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
  return srcID;
}

void FrontendInvocation::RecordPrimarySourceID(unsigned primarySourceID) {
  stone::Panic("RecordPrimarySourceID not implemented");
}

std::unique_ptr<OutputFile>
FrontendInvocation::ComputeOutputFile(FrontendUnit &source) {
  stone::Panic("ComputeSourceOutputFile not implemented");
}