#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"

namespace stone {

// Platform-independent implementation of Task,
// a particular platform can provide its own more efficient version.
class Task : public DriverAllocation<Task> {
public:
  /// The path to the executable which this Task will execute.
  const char *ExecPath;

  /// Any arguments which should be passed during execution.
  llvm::ArrayRef<const char *> Args;

  /// The environment which should be used during execution. If empty,
  /// the current process's environment will be used instead.
  llvm::ArrayRef<const char *> Env;

  /// Context associated with this Task.
  void *Context;

  /// True if the errors of the Task should be stored in Errors instead of
  /// Output.
  bool SeparateErrors;

  llvm::SmallString<64> StdoutPath;

  llvm::SmallString<64> StderrPath;

  llvm::sys::ProcessInfo PI;
};

} // namespace stone