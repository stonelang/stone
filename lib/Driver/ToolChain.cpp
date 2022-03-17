#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Driver.h"

using namespace stone;

ToolChain::ToolChain(ToolChainKind kind, const Driver &driver,
                     const llvm::Triple &triple)
    : kind(kind), driver(driver), triple(triple) {}

bool ToolChain::Initialize() {

  // TODO: Clean this up -- works for now
  auto stoneTool = BuildSCTool();
  if (stoneTool) {
    tools.Add(std::move(stoneTool));
    if (driver.JustCompile()) {
      return true;
    }
  }
  auto ldTool = BuildLDTool();
  if (ldTool) {
    tools.Add(std::move(ldTool));
  }
  auto lldTool = BuildLLDTool();
  if (lldTool) {
    tools.Add(std::move(lldTool));
  }
  auto clangTool = BuildClangTool();
  if (clangTool) {
    tools.Add(std::move(clangTool));
  }
  auto gccTool = BuildGCCTool();
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

std::unique_ptr<Tool> ToolChain::BuildSCTool() {

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

CommandInvocation
ToolChain::ConstructInvocation(const CompileIntent &intent) const {

  auto tool = FindTool(ToolKind::SC);
  assert(tool && "Could not find stone sc tool!");
}

CommandInvocation
ToolChain::ConstructInvocation(const StaticLinkIntent &intent) const {
  assert(false && "StaticLink construction not implemented in base!");
}

CommandInvocation
ToolChain::ConstructInvocation(const DynamicLinkIntent &intent) const {
  assert(false && "DynamicLink construction not implemented in base!");
}

std::unique_ptr<Job>
ToolChain::ConstructJob(const Intent &intent, Compilation &c,
                        const OutputOptions &outputOptions) {

  auto commandInvocation = [&]() -> CommandInvocation {
    switch (intent.GetKind()) {
    case IntentKind::Compile:
      return ConstructInvocation(llvm::cast<CompileIntent>(intent));
    case IntentKind::StaticLink:
      return ConstructInvocation(llvm::cast<StaticLinkIntent>(intent));
    case IntentKind::DynamicLink:
      return ConstructInvocation(llvm::cast<DynamicLinkIntent>(intent));
    case IntentKind::Process:
    case IntentKind::None:
      stone::Panic("Not a 'CompilationIntent' ");
    }
  };
}

// CommandInvocation ToolChain::ConstructInvocation(const CompileIntent &intent)
// {

//   auto tool = driver.GetToolChain().FindTool(ToolKind::Stone);
//   CommandInvocation cmdInvocation(tool);
// }

// Job *ToolChain::CreateCompileJob(Driver &driver) {

//   auto tool = driver.GetToolChain().FindTool(ToolKind::Stone);
//   assert(tool && "Could not find stone sc tool!");
//   return driver.GetCompilation().CreateJob<CompileJob>(driver.GetContext(),
//                                                        *tool);
// }
