#include "stone/Driver/Intent.h"
#include "stone/Core/File.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

const char *Intent::GetNameByKind(IntentKind kind) {
  switch (kind) {
  case IntentKind::Compile:
    return "compile";
  case IntentKind::DynamicLink:
    return "dynamic-link";
  case IntentKind::StaticLink:
    return "static-link";
  case IntentKind::ExecutableLink:
    return "executable-link";
  default:
    assert(false && "Invalid Intention");
  }
}

static void TryBuildMergeModuleIntent() {
  // Intent *mergeModuleIntent = nullptr;
  // if (OI.ShouldGenerateModule &&
  //     OI.compileModelKind !=
  //     DriverOutputContext::CompileModel::SingleCompile &&
  //     !AllModuleInputs.empty()) {
  //   // We're performing multiple compilations; set up a merge module step
  //   // so we generate a single swiftmodule as output.
  //   mergeModuleIntent = std::move(ibc.moduleInputs).IntoIntent(ibc.current);
  // }
}
static void BuildCompileIntent(Driver &driver, BuildCompilationCache &bcc,
                               Intent *intent) {
  assert(intent);
  // TODO: GetLastBuildState
  // bool isTopLvel = (driver.IsLinkable()) ? false : true;
  // bi.current =
  //     driver.GetCompilation().CreateIntent<CompileIntent>(intent, isTopLvel);

  // // TODO: driver.GetDriverOptions().outputFileType);
  // // driver.GetBuildSystem().GetLastBuildState());

  // // TODO: Think about
  // if (!isTopLvel) {
  //   bi.linkerInputs.push_back(bi.current);
  // }
}

static void BuildLinkIntent(Driver &driver, BuildCompilationCache &bcc,
                            Intent *intent) {
  // if (driver.IsLinkable()) {
  //   bi.likerInputs.push_back(intent);
  // }
}

static void BuildIntent(Driver &driver, BuiltIntents &bi, file::File &input) {

  // bi.current = driver.GetCompilation().CreateIntent<ProcessIntent>(input);
  // assert(bi.current);

  // switch (input.GetType()) {
  // case file::Type::Stone:
  //   BuildCompileIntent(driver, bi, bi.current);
  //   break;
  // case file::Type::Object:
  //   BuildLinkIntent(driver, bi, bi.current);
  //   break;
  // }
}

void Driver::BuildIntents(BuildCompilationCache &bcc) {

  for (auto &input : GetOptions().inputFiles) {

    // if (GetBuild().IsDirty(input)) {

    //   assert(input.GetType() == GetInputFileType() &&
    //          "Incompatible input file types");

    //   assert(file::IsPartOfCompilation(input.GetType()));

    //   BuildIntent(*this, bi, input);
    // }
  }
  // We are in optional territory -- try to build

  // TryBuildMergeModuleIntent(bi);

  // TryBuildLinkIntent(bi);

  // bool shouldPerformLTO = OI.LTOVariant != OutputInfo::LTOKind::None;
  // if (OI.ShouldLink() && !AllLinkerInputs.empty()) {
  //   JobAction *LinkAction = nullptr;
}