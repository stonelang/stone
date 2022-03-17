#include "stone/Compile/Lang.h"
#include "stone/Core/Context.h"
#include "stone/Core/Malloc.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/SystemOptions.h"
#include "stone/Option/ModeKind.h"

#include "gtest/gtest.h"

using namespace stone;

class CompileTest : public ::testing::Test {
protected:
  // Context ctx;
};

TEST_F(CompileTest, CompileFileVerbosely) {

  const char *args[] = {"-parse", "/var/tmp/test.stone"};

  Frontend lc;
  lc.ParseArguments(args);

  // Lang lang("stone-compile", "stone-compile");
  // lang.Initialize();

  // auto &ial = lang.ParseArguments(args);
  // auto &tal = lang.TranslateInputArgList(ial);

  // lang.ComputeModeKind(tal);
  // ASSERT_EQ(ModeKind::Parse, lang.GetMode().GetKind());

  // lang.BuildInputFiles(tal);
  // // lang.AddFile("/Users/severin/code/stone/build/tes0.stone");

  // assert(lang.GetOptions().inputFiles.size() > 0);

  // auto sf = lang.ParseFile(lang.GetOptions().inputFiles[0]);

  // lang.TypeCheckSyntaxFile(*sf);

  // lang.Build(args); or call separately
  // lang.ComputeMode();

  // auto options = lang.BuildOptions(args);
  // lang.BuildOptions(args);
  // lang.BuildFiles(args);

  // //auto files = lang.BuildFiles(options.GetFiles());
  // lang.CompileFiles(options.GetFiles()); //-> lang.CompileFile(file)
  // lang.CompileFiles(lang.GetOptions().GetFiles());

  // lang.ParseFile(file);

  // lang.GetLangOptions().AddFile("test.stone");
  // lang.Build();
  // lang.Run();
}

TEST_F(CompileTest, CompileFile) {

  // Lang lang("test", "test");
  // lang.Initialize();

  // const char *args[] = {"-parse", "test.stone"};

  // auto &ial = lang.ParseArguments(args);
  // lang.BuildSession(ial);
}
