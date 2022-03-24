#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

ToolChain::ToolChain(ToolChainKind kind, Driver &driver,
                     const llvm::Triple &triple)
    : kind(kind), driver(driver), triple(triple) {}

bool ToolChain::Initialize() {
  // TODO: Clean this up -- works for now
  auto sc = BuildSC();
  if (sc) {
    tools.Add(std::move(sc));
    if (driver.JustCompile()) {
      return true;
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
  auto clangTool = BuildClang();
  if (clangTool) {
    tools.Add(std::move(clangTool));
  }
  auto gccTool = BuildGCC();
  if (gccTool) {
    tools.Add(std::move(gccTool));
  }
  return false;
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
                  "stone-compile", true);
    if (tool) {
      return tool;
    }
  }
  return nullptr;
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
  //     ComputeResponseFileDetail(compilation, executablePath, jobDetail, context);

  return nullptr;
}
