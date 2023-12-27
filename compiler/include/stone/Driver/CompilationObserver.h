#ifndef STONE_DIRVER_COMPILATIONLISTENER_H
#define STONE_DIRVER_COMPILATIONLISTENER_H

#include "stone/Basic/File.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Task;
class Compilation;
class CompilationObserver {
public:
  // The start of the compilation
  virtual void CompilationStarted(Compilation *compilation) {}

  // Notificatin that the job has started
  virtual void TaskStarted(Task *job) {}

  // Notification that the job has finished.
  virtual void TaskFinished(Task *job) {}

  // Notification that the compilation has completed.
  virtual void CompilationCompleted(Compilation *compilation) {}
};

} // namespace stone
#endif