#include "stone/Driver/Task.h"

Task::Task(const char *execPath, llvm::ArrayRef<const char *> args,
           llvm::ArrayRef<const char *> env = llvm::None,
           void *context = nullptr, bool separateErrors = false)
    : execPath(execPath), args(args), env(env), context(context),
      separateErrors(separateErrors) {}

/// Begins execution of this Task.
/// \returns true on error.
bool Task::Execute() { return false; }