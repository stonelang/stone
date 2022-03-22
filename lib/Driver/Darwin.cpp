#include "stone/Driver/Darwin.h"

#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

Darwin::Darwin(Driver &driver, const llvm::Triple &triple,
               const llvm::Optional<llvm::Triple> &targetVariant)
    : ToolChain(ToolChainKind::Darwin, driver, triple),
      targetVariant(targetVariant) {
  Initialize();
}

bool Darwin::Initialize() {
  libPaths.push_back("/usr/lib/");
  return ToolChain::Initialize();
}

std::unique_ptr<Tool> Darwin::BuildSCTool() {
  // Check to see if the SC path was given by the user.
  auto tool = ToolChain::BuildSCTool();
  if (tool) {
    return tool;
  }
  tool =
      BuildTool(ToolKind::SC, "/usr/bin/stone-compile", "stone-compile", true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::SC, "/usr/local/bin/stone-compile",
                   "stone-compile", false);
  if (tool) {
    return tool;
  }
  return nullptr;
}

std::unique_ptr<Tool> Darwin::BuildLDTool() {
  auto tool = BuildTool(ToolKind::LD, "/usr/bin/ld", "ld", true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::LD, "/usr/local/bin/ld", "ld", false);
  if (tool) {
    return tool;
  }

  return nullptr;
}

std::unique_ptr<Tool> Darwin::BuildLLDTool() {
  auto tool = BuildTool(ToolKind::LLD, "/usr/bin/lld", "lld", true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::LLD, "/usr/local/bin/lld", "lld", false);
  if (tool) {
    return tool;
  }
  return nullptr;
}
std::unique_ptr<Tool> Darwin::BuildClangTool() {
  auto tool = BuildTool(ToolKind::Clang, "/usr/bin/clang++", "clang++", true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::Clang, "/usr/local/bin/clang++", "clang++", false);
  if (tool) {
    return tool;
  }
  return nullptr;
}
std::unique_ptr<Tool> Darwin::BuildGCCTool() {
  auto tool = BuildTool(ToolKind::GCC, "/usr/bin/g++", "g++", true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::GCC, "/usr/local/bin/g++", "g++", false);
  if (tool) {
    return tool;
  }
  return nullptr;
}

Job *Darwin::ConstructCompileJob(Compilation &compilation,
                                 const file::File &input,
                                 const OutputOptions &outputOpts) {
  return ToolChain::ConstructCompileJob(compilation, input, outputOpts);
}

Job *Darwin::ConstructStaticLinkJob(job::InputList inputs,
                                    const OutputOptions &outputOpts) {
  auto tool = FindTool(ToolKind::LD);
  if (!tool) {
    tool = FindTool(ToolKind::LLD);
  }
  assert(tool && "Could not find a linker tool");
  // return std::make_unique<StaticLinkJob>(driver.GetContext(), *tool, inputs);
  return nullptr;
}

Job *Darwin::ConstructDynamicLinkJob(job::InputList inputs,
                                     const OutputOptions &outputOpts) {
  return nullptr;
}

Job *Darwin::ConstructExecLinkJob(job::InputList inputs,
                                  const OutputOptions &outputOpts) {
  return nullptr;
}

// JobInvocation
// Darwin::ConstructInvocation(const CompileJobRequest &request) const
// {
//   return ToolChain::ConstructInvocation(request);
// }

// JobInvocation
// Darwin::ConstructInvocation(const LinkJobRequest &request) const {
//   auto tool = FindTool(ToolKind::LD);
//   if (!tool) {
//     tool = FindTool(ToolKind::LLD);
//   }
//   assert(tool && "Could not find a linker tool");
//   return JobInvocation(request, *tool);
// }

// Job *Darwin::CreateCompileJob(Driver &driver) {
//   return ToolChain::CreateCompileJob(driver);
// }

// Job *Darwin::CreateLinkJob(Driver &driver) {

//   Tool *tool = nullptr;
//   if (driver.GetDriverOptions().useLDLinker) {
//     tool = driver.GetToolChain().FindTool(ToolKind::LD);
//     if (!tool) {
//       tool = driver.GetToolChain().FindTool(ToolKind::LLD);
//     }
//   } else if (driver.GetDriverOptions().useLLDLinker) {
//     tool = driver.GetToolChain().FindTool(ToolKind::LLD);
//     if (!tool) {
//       tool = driver.GetToolChain().FindTool(ToolKind::LD);
//     }
//   }
//   assert(tool && "Could not find either 'ld' or 'lld' linker tools");

//   switch (driver.GetLinkMode()) {
//   case LinkMode::EmitExecutable: {
//     return driver.GetCompilation().CreateJob<ExecutableLinkJob>(
//         driver.GetContext(), *tool, driver.GetDriverOptions().RequiresLTO());
//   }
//   case LinkMode::EmitDynamicLibrary: {
//     return driver.GetCompilation().CreateJob<DynamicLinkJob>(
//         driver.GetContext(), *tool, driver.GetDriverOptions().RequiresLTO());
//   }
//   case LinkMode::EmitStaticLibrary: {
//     return driver.GetCompilation().CreateJob<StaticLinkJob>(
//         driver.GetContext(), *tool, driver.GetDriverOptions().RequiresLTO());
//   }
//   default:
//     assert(false && "Invalid linking kind");
//   }
// }
// Job *Darwin::CreateMergeModuleJob(Driver &driver) {
// return nullptr; }

// void Darwin::AddTool(Tool& tool){
//   tools.push_back(tool)
// }
