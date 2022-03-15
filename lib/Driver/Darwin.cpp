#include "stone/Driver/Darwin.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

DarwinToolChain::DarwinToolChain(
    const Driver &driver, const llvm::Triple &triple,
    const llvm::Optional<llvm::Triple> &targetVariant)
    : ToolChain(ToolChainKind::Darwin, driver, triple),
      targetVariant(targetVariant) {}

bool DarwinToolChain::Initialize() {

  libPaths.push_back("/usr/lib/");

  return ToolChain::Initialize();
}

std::unique_ptr<Tool> DarwinToolChain::BuildSCTool() {

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

std::unique_ptr<Tool> DarwinToolChain::BuildLDTool() {

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

std::unique_ptr<Tool> DarwinToolChain::BuildLLDTool() {

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
std::unique_ptr<Tool> DarwinToolChain::BuildClangTool() {

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
std::unique_ptr<Tool> DarwinToolChain::BuildGCCTool() {

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

CommandInvocation
DarwinToolChain::ConstructInvocation(const CompileIntent &intent) const {
  return ToolChain::ConstructInvocation(intent);
}
CommandInvocation
DarwinToolChain::ConstructInvocation(const StaticLinkIntent &intent) const {

  auto tool = FindTool(ToolKind::LD);
  if (!tool) {
    tool = FindTool(ToolKind::LLD);
  }
  assert(tool && "Could not find a linker tool");
}

CommandInvocation
DarwinToolChain::ConstructInvocation(const DynamicLinkIntent &intent) const {

  auto tool = FindTool(ToolKind::LD);
  /// Warn if you are asked to use LD only
  if (!tool) {
    tool = FindTool(ToolKind::LLD);
  }
  assert(tool && "Could not find a linker tool");

  // assert(tool && "Could not find stone sc tool!");
}

// Job *DarwinToolChain::CreateCompileJob(Driver &driver) {
//   return ToolChain::CreateCompileJob(driver);
// }

// Job *DarwinToolChain::CreateLinkJob(Driver &driver) {

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

//   switch (driver.GetLinkKind()) {
//   case LinkKind::EmitExecutable: {
//     return driver.GetCompilation().CreateJob<ExecutableLinkJob>(
//         driver.GetContext(), *tool, driver.GetDriverOptions().RequiresLTO());
//   }
//   case LinkKind::EmitDynamicLibrary: {
//     return driver.GetCompilation().CreateJob<DynamicLinkJob>(
//         driver.GetContext(), *tool, driver.GetDriverOptions().RequiresLTO());
//   }
//   case LinkKind::EmitStaticLibrary: {
//     return driver.GetCompilation().CreateJob<StaticLinkJob>(
//         driver.GetContext(), *tool, driver.GetDriverOptions().RequiresLTO());
//   }
//   default:
//     assert(false && "Invalid linking kind");
//   }
// }
// Job *DarwinToolChain::CreateMergeModuleJob(Driver &driver) {
// return nullptr; }

// void DarwinToolChain::AddTool(Tool& tool){
//   tools.push_back(tool)
// }
