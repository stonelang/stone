#ifndef STONE_DRIVER_BUILDSYSTEM_H
#define STONE_DRIVER_BUILDSYSTEM_H

#include "stone/Basic/File.h"
#include "stone/Basic/LLVM.h"
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

class BuildFile {};

class BuildSolution {};
class BuildProject {};
class BuildSystem final {
  Driver &driver;
  // saveTempFiles

public:
  BuildSystem(Driver &driver);
  ~BuildSystem();

  void Initialize();

public:
  // Create a unique hash key for this file(/path/file.stone) and save to build
  // history
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
