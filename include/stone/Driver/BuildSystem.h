#ifndef STONE_DRIVER_BUILDSYSTEM_H
#define STONE_DRIVER_BUILDSYSTEM_H

#include "stone/Core/File.h"
#include "stone/Core/LLVM.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Chrono.h"

namespace stone {

class Driver;

class BuildState final {
public:
};

// class BuildEntry {
// public:
//   void PrevTime();
//   void CurrTime();
// };

class BuildHistory final {
  std::string path;

public:
  llvm::sys::TimePoint<> GetLastTime();
};

class BuildSystem final {
  Driver &driver;
  // saveTempFiles

public:
  BuildSystem(Driver &driver);
  ~BuildSystem();

  void Initialize();

public:
  bool IsDirty(const file::File &entry);
  void Clean();

public:
  llvm::sys::TimePoint<> GetStartTime();

public:
  void StartBuild();
  void StopBuild();
};

} // namespace stone
#endif
