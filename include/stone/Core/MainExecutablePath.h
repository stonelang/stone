#ifndef STONE_CORE_MAINEXECUTABLEPATH_H
#define STONE_CORE_MAINEXECUTABLEPATH_H

#include "llvm/Support/FileSystem.h"

namespace stone {
inline std::string GetMainExecutablePath(const char *arg0) {
  void *mainExecPath = (void *)(intptr_t)stone::GetMainExecutablePath;
  return llvm::sys::fs::getMainExecutable(arg0, mainExecPath);
}
} // namespace stone
#endif
