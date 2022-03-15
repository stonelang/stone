#include "stone/Driver/Command.h"
#include "stone/Core/Context.h"
#include "stone/Driver/ToolChain.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;

CommandOutput::CommandOutput(file::Type primaryOutputType, OutputFileMap &map)
    : derivedOutputMap(map) {}

CommandOutput::CommandOutput(llvm::StringRef fakeBaseName, OutputFileMap &map)
    : derivedOutputMap(map) {}

int cmd::ExecuteSync(Command &c, Context *ctx) {

  assert(c.waitSecs > 0 && "Wait seconds must be greater than 0");
  return llvm::sys::ExecuteAndWait(
      c.GetTool().GetFullName(), llvm::ArrayRef<llvm::StringRef>(c.args), c.env,
      c.redirects, c.waitSecs, c.memLimit, c.errMsg, c.failed);
}
int cmd::ExecuteAsync(Command &c, Context *ctx) {

  assert(c.waitSecs == 0 && "Wait seconds must be equal to 0");
  return llvm::sys::ExecuteAndWait(
      c.GetTool().GetFullName(), llvm::ArrayRef<llvm::StringRef>(c.args), c.env,
      c.redirects, c.waitSecs, c.memLimit, c.errMsg, c.failed);
}
