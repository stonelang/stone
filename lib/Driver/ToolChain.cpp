#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"
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

Job *ToolChain::ConstructCompileJob(Compilation &compilation,
                                    const file::File &input,
                                    const OutputOptions &outputOpts) {

  auto tool = FindTool(ToolKind::SC);
  assert(tool);
  
  auto job = compilation.CreateJob<CompileJob>(driver.GetContext(), *tool,
                                               const_cast<file::File *>(&input),
                                               outputOpts.outputFileType);
  return job;
}

Job *ToolChain::ConstructStaticLinkJob(job::InputList inputs,
                                       const OutputOptions &outputOpts) {
  return nullptr;
}

Job *ToolChain::ConstructDynamicLinkJob(job::InputList inputs,
                                        const OutputOptions &outputOpts) {
  return nullptr;
}

Job *ToolChain::ConstructExecLinkJob(job::InputList inputs,
                                     const OutputOptions &outputOpts) {
  return nullptr;
}

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
