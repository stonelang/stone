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
  //     DriverOutputContext::CompilingModel::SingleCompile &&
  //     !AllModuleInputs.empty()) {
  //   // We're performing multiple compilations; set up a merge module step
  //   // so we generate a single swiftmodule as output.
  //   mergeModuleIntent = std::move(ibc.moduleInputs).IntoIntent(ibc.current);
  // }
}
static void BuildCompileIntent(Compilation &compilation,
                               CompilationHotInfo &chi, Intent *intent) {
  assert(intent);

  // auto IsTopLevel = [&]() -> bool {
  //   bool isTopLevel = (compilation.GetDriver().CanLink()) ? false : true;
  //   chi.current = compilation.CreateIntent<CompileIntent>(intent,
  //   IntentLevel::); return isTopLevel;
  // };
  // if (IsTopLevel()) {
  //   chi.topLevelIntents.push_back(chi.current);
  // } else {
  //   chi.linkerInputs.push_back(chi.current);
  // }

  auto level =
      (compilation.GetDriver().CanLink()) ? IntentLevel::Dep : IntentLevel::Top;
  chi.current = compilation.CreateIntent<CompileIntent>(level, intent);

  // Think about
  // if (IsTopLevel()) {
  //   chi.topLevelIntents.push_back(chi.current);
  // } else {
  //   chi.linkerInputs.push_back(chi.current);
  // }

  // chi.linkerInputs.push_back(chi.current);

  // TODO: GetLastBuildState
  // bool isTopLevel = (driver.CanLink()) ? false : true;
  // bcs.current = compilation.CreateIntent<CompileIntent>(intent, isTopLevel);

  // TODO: driver.GetDriverOptions().outputFileType);
  // driver.GetBuildSystem().GetLastBuildState());

  // TODO: Think about
  // if (!isTopLevel) {
  //   bcs.linkerInputs.push_back(bcs.current);
  // }
}

static void BuildLinkIntent(Compilation &compilation, CompilationHotInfo &chi,
                            Intent *intent) {

  if (compilation.GetDriver().CanLink()) {
    chi.linkerInputs.push_back(intent);
  }
}

static void BuildIntent(Compilation &compilation, CompilationHotInfo &chi,
                        const file::File &input) {

  chi.current = compilation.CreateIntent<ProcessIntent>(input);
  assert(chi.current);
  switch (input.GetType()) {
  case file::Type::Stone:
    BuildCompileIntent(compilation, chi, chi.current);
    break;
  case file::Type::Object:
    BuildLinkIntent(compilation, chi, chi.current);
    break;
  default:
    stone::Panic("Alien file type whilst builid intent");
  }
}

static void BuildIntentForMultipleCompilingModel(Compilation &compilation,
                                                 CompilationHotInfo &chi,
                                                 const file::File input) {}

static void BuildIntentForSingleCompilingModel(Compilation &compilation,
                                               CompilationHotInfo &chi,
                                               const file::File &input) {}

void Driver::BuildIntents(Compilation &compilation, CompilationHotInfo &chi,
                          const file::Files &inputs) {

  for (auto &input : inputs) {
    // TODO: Way out there, but there is a potential for git here?
    if (GetBuildSystem().IsDirty(input)) {

      assert(input.GetType() == GetInputFileType() &&
             "Incompatible input file types");
      assert(file::IsPartOfCompilation(input.GetType()));

      switch (driverOpts.outputOptions.compilingModel) {
      case CompilingModel::Multiple:
        BuildIntentForMultipleCompilingModel(compilation, chi, input);
        break;
      case CompilingModel::Single:
        BuildIntentForSingleCompilingModel(compilation, chi, input);
        break;
      default:
        stone::Panic("Unsupported Compiling mode");
      }
    }
  }
  // We are in optional territory -- try to build

  // TryBuildMergeModuleIntent(bi);

  // TryBuildLinkIntent(bi);

  // bool shouldPerformLTO = OI.LTOVariant != OutputInfo::LTOKind::None;
  // if (OI.ShouldLink() && !AllLinkerInputs.empty()) {
  //   JobAction *LinkAction = nullptr;
}