#include "stone/Drive/Driver.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Drive/CompilationJob.h"
#include "stone/Drive/Phase.h"
#include "stone/LangContext.h"
#include "gtest/gtest.h"

using namespace stone;

class DriverTest : public ::testing::Test {
public:
  LangContext ctx;
  const ToolChain *GetToolChain() const { return nullptr; }
};

// TEST_F(DriverTest, CompileJobTest) {

//   auto file = std::make_unique<file::File>("test", file::Type::Stone);
//   auto tool =
//       std::make_unique<Tool>(ToolKind::SC, "test", "test", *GetToolChain());

//   CompileJob cj(*tool, file.get());
// }

TEST_F(DriverTest, PhaseTest) {
  auto file = std::make_unique<file::File>("test", file::Type::Stone);

  // auto cjr = Driver::MakePhase<CompilePhaset>(file.get());
  // cjr->Print(ctx.Out());

  // auto djrd = Driver::MakePhase<DynamicLinkPhase>(cjr.get());
  // auto djri = Driver::MakePhase<DynamicLinkJobRequest>(file.get());

  // Compilation compilation;
  // auto job = toolChain.MakeJob(JobRequest& jobRequest, Compilation& c)
  // compilation.EnqueuJob(job);

  // auto file = std::make_unique<file::File>("test", file::Type::Stone);
  // auto tool =
  //     std::make_unique<Tool>(ToolKind::SC, "test", "test", *GetToolChain());

  // CompileJob cj(*tool, file.get());
}
// TEST_F(DriverTest, ProcessPhase) {

// Driver driver("stone", "stone");
// driver.Initialize();

// const char *args[] = {"-parse", "test.stone"};
// auto &ial = driver.ParseArguments(args);

// driver.BuildSession(ial);
// auto &compilation = driver.BuildCompilation(ial);
// auto &queue = driver.BuildTaskQueue();

// compilation.Run(queue);

// auto driverOptions = driver.BuildOptions(*parsedArgs.get());
// driver.Build(*parsedArgs.get()) // all of this can be done here
// auto tc = driver.BuildToolChain(*parsedArgs.get())
// auto c = driver.BuildCompilation(*tc.get(), parsedArgs);
//}

// TEST_F(DriverTest, CompilePhase) {}
// TEST_F(DriverTest, LinkPhase) {}
