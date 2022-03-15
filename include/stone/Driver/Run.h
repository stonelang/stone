#ifndef STONE_COMPILATION_RUN_H
#define STONE_COMPILATION_RUN_H

#include "stone/Core/File.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Compilation;
class CompilationListener {
public:
  virtual void OnCompilationStarted(Compilation &cc) {}
  virtual void OnCompilationCompleted(Compilation &cc) {}
};

namespace driver {
int Run(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
        CompilationListener *listener);
}
} // namespace stone
#endif
