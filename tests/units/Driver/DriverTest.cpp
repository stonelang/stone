#include "stone/Driver/Driver.h"
#include "stone/Core/Context.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/SystemOptions.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/JobRequest.h"

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

TEST_F(DriverTest, JobRequestTest) {

  auto file = std::make_unique<file::File>("test", file::Type::Stone);

  auto cjr = JobRequest::Make<CompileJobRequest>(file.get());
  cjr->Print(ctx.Out());

  auto djrd = JobRequest::Make<DynamicLinkJobRequest>(cjr.get());
  auto djri = JobRequest::Make<DynamicLinkJobRequest>(file.get());

  // Compilation compilation;
  // auto job = toolChain.MakeJob(JobRequest& jobRequest, Compilation& c)
  // compilation.EnqueuJob(job);

  // auto file = std::make_unique<file::File>("test", file::Type::Stone);
  // auto tool =
  //     std::make_unique<Tool>(ToolKind::SC, "test", "test", *GetToolChain());

  // CompileJob cj(*tool, file.get());
}
// TEST_F(DriverTest, ProcessIntent) {

// Driver driver("stone", "stone");
// driver.Initialize();

// const char *args[] = {"-parse", "test.stone"};
// auto &ial = driver.ParseArguments(args);

// driver.BuildSession(ial);
// auto &compilation = driver.BuildCompilation(ial);
// auto &queue = driver.BuildJobQueue();

// compilation.Run(queue);

// auto driverOptions = driver.BuildOptions(*parsedArgs.get());
// driver.Build(*parsedArgs.get()) // all of this can be done here
// auto tc = driver.BuildToolChain(*parsedArgs.get())
// auto c = driver.BuildCompilation(*tc.get(), parsedArgs);
//}

// TEST_F(DriverTest, CompileIntent) {}
// TEST_F(DriverTest, LinkIntent) {}
