#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

ToolChain::ToolChain(ToolChainKind kind, Driver &driver,
                     const llvm::Triple &triple)
    : kind(kind), driver(driver), triple(triple) {
  Initialize();
}

void ToolChain::Initialize() {
  // TODO: Clean this up -- works for now
  auto sc = BuildSC();
  if (sc) {
    tools.Add(std::move(sc));
    if (driver.JustCompile()) {
      return;
    }
  }
  auto ld = BuildLD();
  if (ld) {
    tools.Add(std::move(ld));
  }
  auto lld = BuildLLD();
  if (lld) {
    tools.Add(std::move(lld));
  }
  auto clang = BuildClang();
  if (clang) {
    tools.Add(std::move(clang));
  }
  auto gcc = BuildGCC();
  if (gcc) {
    tools.Add(std::move(gcc));
  }
}

std::unique_ptr<Tool> ToolChain::BuildTool(ToolKind kind, const char *fullName,
                                           const char *shortName,
                                           bool isDefault) {
  auto tool =
      std::make_unique<Tool>(kind, fullName, shortName, *this, isDefault);
  if (tool->OnSystem()) {
    if (tool->CanExec()) {
      return tool;
    }
  }
  return nullptr;
}

Tool *ToolChain::FindTool(ToolKind tk) const {
  for (auto &tool : tools) {
    if (tool.GetKind() == tk) {
      return &tool;
    }
  }
  return nullptr;
}

std::unique_ptr<Tool> ToolChain::BuildSC() {
  if (driver.GetDriverOptions().HasSCPath()) {
    auto tool =
        BuildTool(ToolKind::SC, driver.GetDriverOptions().scPath.c_str(),
                  ToolName::SC, true);
    if (tool) {
      return tool;
    }
    // Try to find relative to running path
    // std::string relativeToStone =
    //     FindProgramRelativeToStone(invocationInfo.ExecutableName);

    // tool = BuildTool(ToolKind::SC, relativeToStone.c_str(), ToolName::SC,
    // true); if (tool) {
    //   return tool;
    // }
  }
  return nullptr;
}

std::string
ToolChain::FindProgramRelativeToStone(llvm::StringRef executableName) const {
  // auto insertionResult =
  //     ProgramLookupCache.insert(std::make_pair(executableName, ""));
  // if (insertionResult.second) {
  //   std::string path = FindProgramRelativeToSwiftImpl(executableName);
  //   insertionResult.first->setValue(std::move(path));
  // }
  // return insertionResult.first->getValue();
  return {};
}

std::string ToolChain::FindProgramRelativeToStoneImpl(
    llvm::StringRef executableName) const {
  // llvm::StringRef stonePath = GetDriver().GetSwiftProgramPath();
  // llvm::StringRef stoneBinDir = llvm::sys::path::parent_path(stonePath);
  // auto result = llvm::sys::findProgramByName(executableName, {stoneBinDir});
  // if (result) {
  //   return result.get();
  // }
  return {};
}

JobDetail ToolChain::ConstructDetail(const CompileIntent &intent) {
  return JobDetail();
}

std::unique_ptr<Job>
ToolChain::ConstructJob(const Intent &intent, Compilation &compilation,
                        std::unique_ptr<CommandOutput> output,
                        const OutputOptions &outputOptions) {

  auto jobDetail = [&]() -> JobDetail {
    switch (intent.GetKind()) {
    case IntentKind::Compile:
      return ConstructDetail(llvm::cast<CompileIntent>(intent));
    case IntentKind::StaticLink:
      return ConstructDetail(llvm::cast<StaticLinkIntent>(intent));
    case IntentKind::DynamicLink:
      return ConstructDetail(llvm::cast<DynamicLinkIntent>(intent));
    case IntentKind::ExecutableLink:
      return ConstructDetail(llvm::cast<ExecutableLinkIntent>(intent));
    default:
      stone::Panic("No 'Intent' to build Job");
    }
  }();

  // Determine if the argument list is so long that it needs to be written into
  // a response file.
  // auto responseFileDetail =
  //     ComputeResponseFileDetail(compilation, executablePath, jobDetail,
  //     context);

  return nullptr;
}
