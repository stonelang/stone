#include "stone/Driver/DarwinToolChain.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::darwin;

DarwinToolChain::DarwinToolChain(
    Driver &driver, const llvm::Triple &triple,
    const llvm::Optional<llvm::Triple> &targetVariant)
    : ToolChain(ToolChainKind::Darwin, driver, triple),
      targetVariant(targetVariant) {}

void DarwinToolChain::Initialize() { libPaths.push_back("/usr/lib/"); }

std::unique_ptr<Tool> DarwinToolChain::BuildSC() {

  // Check to see if the SC path was given by the user.
  auto tool = ToolChain::BuildSC();
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::SC, "/usr/bin/stone-compile", ToolName::SC, true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::SC, "/usr/local/bin/stone-compile", ToolName::SC,
                   false);
  if (tool) {
    return tool;
  }
  stone::Panic("Could not find 'stone-compile'");
}

std::unique_ptr<Tool> DarwinToolChain::BuildLD() {
  auto tool = BuildTool(ToolKind::LD, "/usr/bin/ld", ToolName::LD, true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::LD, "/usr/local/bin/ld", ToolName::LD, false);
  if (tool) {
    return tool;
  }

  return nullptr;
}

std::unique_ptr<Tool> DarwinToolChain::BuildLLD() {
  auto tool = BuildTool(ToolKind::LLD, "/usr/bin/lld", ToolName::LLD, true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::LLD, "/usr/local/bin/lld", ToolName::LLD, false);
  if (tool) {
    return tool;
  }
  return nullptr;
}
std::unique_ptr<Tool> DarwinToolChain::BuildClang() {
  auto tool =
      BuildTool(ToolKind::Clang, "/usr/bin/clang++", ToolName::Clang, true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::Clang, "/usr/local/bin/clang++", ToolName::Clang,
                   false);
  if (tool) {
    return tool;
  }
  return nullptr;
}
std::unique_ptr<Tool> DarwinToolChain::BuildGCC() {
  auto tool = BuildTool(ToolKind::GCC, "/usr/bin/g++", ToolName::GCC, true);
  if (tool) {
    return tool;
  }
  tool = BuildTool(ToolKind::GCC, "/usr/local/bin/g++", ToolName::GCC, false);
  if (tool) {
    return tool;
  }
  return nullptr;
}

JobDetail DarwinToolChain::ConstructDetail(const CompileIntent &intent) {
  return ToolChain::ConstructDetail(intent);
}

JobDetail DarwinToolChain::ConstructDetail(const DynamicLinkIntent &intent) {

  return JobDetail();
}

JobDetail DarwinToolChain::ConstructDetail(const StaticLinkIntent &intent) {

  return JobDetail();
}

JobDetail DarwinToolChain::ConstructDetail(const ExecutableLinkIntent &intent) {
  return JobDetail();
}

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
// Job *DarwinToolChain::CreateMergeModuleJob(Driver &driver) {
// return nullptr; }

// void DarwinToolChain::AddTool(Tool& tool){
//   tools.push_back(tool)
// }
