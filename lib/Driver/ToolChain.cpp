#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Driver.h"

using namespace stone;

ToolChain::ToolChain(ToolChainKind kind, Driver &driver,
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

std::unique_ptr<Job> ToolChain::ConstructCompileJob(const file::File &input) {
  return nullptr;
}

std::unique_ptr<Job>
ToolChain::ConstructStaticLinkJob(InputList inputs, file::Type outputFileType) {
  return nullptr;
}

std::unique_ptr<Job>
ToolChain::ConstructStaticLinkJob(DepList deps, file::Type outputFileType) {
  return nullptr;
}

std::unique_ptr<Job>
ToolChain::ConstructDynamicLinkJob(InputList inputs, file::Type outputFileType,
                                   bool withLTO) {
  return nullptr;
}
std::unique_ptr<Job>
ToolChain::ConstructDynamicLinkJob(DepList deps, file::Type outputFileType,
                                   bool withLTO) {
  return nullptr;
}

std::unique_ptr<Job>
ToolChain::ConstructExecLinkJob(InputList inputs, file::Type outputFileType) {
  return nullptr;
}

std::unique_ptr<Job>
ToolChain::ConstructExecLinkJob(DepList deps, file::Type outputFileType) {

  return nullptr;
}

// JobInvocation
// ToolChain::ConstructInvocation(const CompileJobRequest &request) const {

//   auto tool = FindTool(ToolKind::SC);
//   assert(tool && "Could not find 'stone-compile' tool!");

//   JobInvocation ji(request, *tool);
//   return ji;
// }

// JobInvocation
// ToolChain::ConstructInvocation(const LinkJobRequest &request) const {
//   stone::Panic("StaticLink construction not implemented  here");
// }

// std::unique_ptr<Job>
// ToolChain::ConstructJob(const JobRequest &request, Compilation &c,
//                         std::unique_ptr<CommandOutput> output,
//                         const OutputOptions &outputOptions) {

//   auto Invocation = [&]() -> JobInvocation {
//     switch (request.GetKind()) {
//     case RequestKind::Compile:
//       return ConstructInvocation(llvm::cast<CompileJobRequest>(request));
//     case RequestKind::Link:
//       return ConstructInvocation(llvm::cast<LinkJobRequest>(request));
//     case RequestKind::Input:
//       stone::Panic("Not a 'JobRequest'");
//     }
//   };
// }

// std::unique_ptr<CompilationJob> ToolChain::ConstructCompileJob() {

//   auto tool = FindTool(ToolKind::SC);
//   return nullptr;
// }

// Job *ToolChain::CreateCompileJob(Driver &driver) {

//   auto tool = driver.GetToolChain().FindTool(ToolKind::Stone);
//   assert(tool && "Could not find stone sc tool!");
//   return driver.GetCompilation().CreateJob<CompileJob>(driver.GetContext(),
//                                                        *tool);
// }
