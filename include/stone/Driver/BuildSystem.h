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

class BuildHistory {};

class BuildEntry {
public:
  void PrevTime();
  void CurrTime();
};

class BuildSystem final {
  Driver &driver;

public:
  BuildSystem(Driver &driver);
  ~BuildSystem();

  void Initialize();

public:
  bool IsDirty(file::File &entry);
  void Clean();
};

} // namespace stone
#endif
