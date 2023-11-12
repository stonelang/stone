#include "stone/Driver/Task.h"

using namespace stone;

sys::Task::Task(const char *execPath, llvm::ArrayRef<const char *> args,
                llvm::ArrayRef<const char *> env, void *context,
                bool separateErrors)
    : execPath(execPath), args(args), env(env), context(context),
      separateErrors(separateErrors) {}

/// Begins execution of this Task.
/// \returns true on error.
stone::Error sys::Task::Execute() { return stone::Error(); }