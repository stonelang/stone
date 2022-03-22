#ifndef STONE_DIRVER_COMPILATIONLISTENER_H
#define STONE_DIRVER_COMPILATIONLISTENER_H

#include "stone/Core/File.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Job;
class Compilation;
class CompilationListener {
public:
  // The start of the compilation
  virtual void OnCompilationStarted(Compilation *c) {}

  // Notificatin that the job has started
  virtual void OnJobStarted(Job *job) {}

  // Notification that the job has finished.
  virtual void OnJobFinished(Job *job) {}

  // Notification that the compilation has completed.
  virtual void OnCompilationCompleted(Compilation *c) {}
};

} // namespace stone
#endif