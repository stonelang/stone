#include "stone/Driver/CompilationModel.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

Intent *
CompilationModel::ConstructCompileIntent(ToolChain &tc, intent::Input input,
                                         const OutputOptions &outputOpts) {
  Intent *intent = nullptr;
  auto sc = tc.GetSC();
  intent = tc.GetDriver().MakeIntent<CompileIntent>(*sc, input,
                                                    outputOpts.outputFileType);
  assert(intent);
  return intent;
}
Intent *CompilationModel::ConstructStaticLinkIntent(
    ToolChain &tc, intent::InputList inputs, const OutputOptions &outputOpts) {

  Intent *intent = nullptr;
  auto ld = tc.GetLD();

  return nullptr;
}

Intent *CompilationModel::ConstructExecLinkIntent(
    ToolChain &tc, intent::InputList inputs, const OutputOptions &outputOpts) {

  auto ld = tc.GetLD();
  return nullptr;
}

Intent *CompilationModel::ConstructDynamicLinkIntent(
    ToolChain &tc, intent::InputList inputs, const OutputOptions &outputOpts) {

  auto ld = tc.GetLD();
  return nullptr;
}

// TODO: Look into the IntentCache instead of the intent::Input
/// Goal: Build the link intent and CacheForTop(..)
Intent *CompilationModel::BuildLinkIntent(ToolChain &tc, IntentCache &ic,
                                          const OutputOptions &outputOpts) {

  Intent *intent = nullptr;
  // Make sure that we can link
  assert(tc.GetDriver().CanLink() &&
         "The current mode does not allow linking.");

  // Make sure that there is stuff to link
  assert(ic.ForCompile() && "There is nothing to link");

  switch (tc.GetDriver().GetLinkMode()) {
  case LinkMode::EmitExecutable:
    intent = ConstructExecLinkIntent(tc, ic.forCompile, outputOpts);
    break;
  case LinkMode::EmitStaticLibrary:
    intent = ConstructStaticLinkIntent(tc, ic.forCompile, outputOpts);
    break;
  case LinkMode::EmitDynamicLibrary:
    intent = ConstructDynamicLinkIntent(tc, ic.forCompile, outputOpts);
    break;
  default:
    stone::Panic("Alien link mode");
  }
  assert(intent);
  ic.CacheForTop(intent);
  return intent;
}

Intent *CompilationModel::BuildLinkIntent(ToolChain &tc,
                                          const file::Files &inputs,
                                          const OutputOptions &outputOpts) {
  Intent *intent = nullptr;

  assert(tc.GetDriver().JustLink() && "The current mode is only for linking");

  return nullptr;
}

void QuadraticCompilationModel::BuildCompileIntents(
    ToolChain &tc, const file::Files &inputs, IntentCache &ic,
    const OutputOptions &outputOpts) {

  auto BuildCompileIntent = [&](const file::File &primaryInput,
                                const file::Files &inputs,
                                const OutputOptions &outputOpts) -> Intent * {
    auto intent = ConstructCompileIntent(
        tc, const_cast<file::File *>(&primaryInput), outputOpts);
    assert(intent);
    for (auto &input : inputs) {
      /// The tool chain stores the intents that it created.
      intent->AddInput(const_cast<file::File *>(&input));
    }
    return intent;
  };
  for (auto &input : inputs) {
    assert(input.GetType() == file::Type::Stone); // Only file-type for now

    auto intent = BuildCompileIntent(input, inputs, outputOpts);
    ic.CacheForCompile(intent);
  }
}
void QuadraticCompilationModel::BuildIntents(ToolChain &tc,
                                             const file::Files &inputs,
                                             IntentCache &ic,
                                             const OutputOptions &outputOpts) {

  if (tc.GetDriver().GetDriverOptions().GetMode().CanCompile()) {
    BuildCompileIntents(tc, inputs, ic, outputOpts);
    if (tc.GetDriver().JustCompile()) {
      return;
    }
  }
  if (tc.GetDriver().CanLink()) {
    if (tc.GetDriver().JustLink()) {
      BuildLinkIntent(tc, inputs, outputOpts);
    } else {
      BuildLinkIntent(tc, ic, outputOpts);
    }
  }
}
void QuadraticCompilationModel::BuildJobs(ToolChain &tc, IntentCache &ic,
                                          const OutputOptions &outputOpts) {

  // auto BuildCompileTaskDetails = [&]() -> void {

  // };

  // if(tc.GetDriver().JustCompile()){

  // }
  // if (tc.GetDriver().GetMode().CanCompile()) {
  //   BuildCompileIntents(driver, tc, inputs, ic, outputOpts);
  //   if (tc.GetDriver().JustCompile()) {
  //     return;
  //   }
  // }
  // if (tc.GetDriver().CanLink()) {
  //   if (tc.GetDriver().JustLink()) {
  //     BuildLinkIntent(driver, tc, inputs, outputOpts);
  //   } else {
  //     BuildLinkIntent(driver, tc, ic, outputOpts);
  //   }
  // }

  // // if we have nothing to do, we return
  // if (ic.ForCompile()) {
  //   return nullptr;
  // }
  // for (auto input : ic.forCompile) {
  //   auto intent = InputToIntent(input);
  //   assert(intent);
  //   auto taskDetail =
  //   tc.ConstructTaskDetail(llvm::cast<CompileIntent>(*intent));
  // }
}
std::unique_ptr<Compilation> QuadraticCompilationModel::BuildCompilation(
    ToolChain &tc, const file::Files &inputs, const OutputOptions &outputOpts) {

  IntentCache ic;
  BuildIntents(tc, inputs, ic, outputOpts);

  BuildJobs(tc, ic, outputOpts);

  // TODO: if print ....

  // TODO: it seems that we can skip these steps if we create the compilation
  // ahead of time and just do
  /// compilation.AddTaskDetail(tc.ConstructTaskDetail(llvm::cast<CompileIntent>(*intent)))

  // TODO: Check input size
  // Now, build the intent system since we have a toolchain
  // auto compilation =
  //     std::make_unique<Compilation>(*this, toolChain, std::move(dal));

  return nullptr;
}

// void FlatCompilationModel::BuildIntents(Compilation& compilation, const
// file::Files &inputs,
//                                      IntentCache &ic,
//                                      const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// FlatCompilationModel::BuildCompilation(Driver &driver,
//                                        const file::Files &inputs, IntentCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {

//   BuildIntents(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void CPUCompilationModel::BuildIntents(Compilation& compilation, const
// file::Files &inputs,
//                                     IntentCache &ic,
//                                     const OutputOptions &outputOpts) {}

// std::unique_ptr<Compilation>
// CPUCompilationModel::BuildCompilation(Driver &driver, const file::Files
// &inputs,
//                                       IntentCache &ic,
//                                       const OutputOptions &outputOpts) {

//   BuildIntents(driver, inputs, ic, outputOpts);
//   return nullptr;
// }

// void SingleCompilationModel::BuildIntents(Compilation& compilation,
//                                        const file::Files &inputs, IntentCache
//                                        &ic, const OutputOptions &outputOpts)
//                                        {}

// std::unique_ptr<Compilation> SingleCompilationModel::BuildCompilation(
//     Driver &driver, const file::Files &inputs, IntentCache &ic,
//     const OutputOptions &outputOpts) {

//   BuildIntents(driver, inputs, ic, outputOpts);
//   return nullptr;
// }
