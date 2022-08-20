#include "stone/Driver/Driver.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Context.h"
#include "stone/Driver/Action.h"
#include "stone/Driver/CompilationJob.h"
#include "gtest/gtest.h"

using namespace stone;

class DriverTest : public ::testing::Test {
public:
  Context ctx;
  const ToolChain *GetToolChain() const { return nullptr; }
};

// TEST_F(DriverTest, CompileJobTest) {

//   auto file = std::make_unique<file::File>("test", file::Type::Stone);
//   auto tool =
//       std::make_unique<Tool>(ToolKind::SC, "test", "test", *GetToolChain());

//   CompileJob cj(*tool, file.get());
// }

TEST_F(DriverTest, ActionTest) {
  auto file = std::make_unique<file::File>("test", file::Type::Stone);

  // auto cjr = Driver::MakeAction<CompileActiont>(file.get());
  // cjr->Print(ctx.Out());

  // auto djrd = Driver::MakeAction<DynamicLinkAction>(cjr.get());
  // auto djri = Driver::MakeAction<DynamicLinkJobRequest>(file.get());

  // Compilation compilation;
  // auto job = toolChain.MakeJob(JobRequest& jobRequest, Compilation& c)
  // compilation.EnqueuJob(job);

  // auto file = std::make_unique<file::File>("test", file::Type::Stone);
  // auto tool =
  //     std::make_unique<Tool>(ToolKind::SC, "test", "test", *GetToolChain());

  // CompileJob cj(*tool, file.get());
}
// TEST_F(DriverTest, ProcessAction) {

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

// TEST_F(DriverTest, CompileAction) {}
// TEST_F(DriverTest, LinkAction) {}
