#ifndef STONE_BASIC_RUN_H
#define STONE_BASIC_RUN_H

#include "stone/Context.h"
#include "stone/Basic/LLVM.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"

namespace stone {
namespace sys {
// Platform-independent implementation of Task,
// a particular platform can provide its own more efficient version.
class Task {
public:
  /// The path to the executable which this Task will execute.
  const char *execPath;

  /// Any arguments which should be passed during execution.
  llvm::ArrayRef<const char *> args;

  /// The environment which should be used during execution. If empty,
  /// the current process's environment will be used instead.
  llvm::ArrayRef<const char *> env;

  /// Context associated with this Task.
  void *context;

  /// True if the errors of the Task should be stored in Errors instead of
  /// Output.
  bool separateErrors;

  llvm::SmallString<64> stdOutPath;

  llvm::SmallString<64> stdErrPath;

  llvm::sys::ProcessInfo processInfo;

  Task(const char *ExecPath, llvm::ArrayRef<const char *> Args,
       llvm::ArrayRef<const char *> Env = llvm::None, void *Context = nullptr,
       bool SeparateErrors = false);
  /// Begins execution of this Task.
  /// \returns true on error.
  stone::Error Execute();
};

} // namespace sys
} // namespace stone

#endif
