#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Driver/DriverAllocation.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {

class Job;
class Driver;
class JobConstruction;
class Compilation;

enum class CompilationEntityKind {
  Input = 0,
};

class CompilationEntity : public DriverAllocation<CompilationEntity> {

public:
  using DriverAllocation<CompilationEntity>::operator new;
  using DriverAllocation<CompilationEntity>::operator delete;

public:
};

class CompilationEntities final {
  friend Driver;
  friend Compilation;

  // A graph of JobConstructions -- do not mark as cons since the
  // JobConstruction creates the Job
  llvm::SmallVector<const JobConstruction *, 8> topLevelJobConstructions;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const Job *, 8> topLevelJobs;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const Job *, 8> topLevelExternalJobs;

  int test = 0;

public:
  void AddTopLevelJobConstruction(const JobConstruction *construction);
  void AddTopLevelJob(const Job *job);
  void AddTopLevelExternalJob(const Job *job);

public:
  bool HasTopLevelJobConstructions() {
    return (!topLevelJobConstructions.empty() &&
            topLevelJobConstructions.size() > 0);
  }

  bool HasTopLevelJobs() {
    return (!topLevelJobConstructions.empty() &&
            topLevelJobConstructions.size() > 0);
  }
  bool HasTopLevelExternalJobs() {
    return (!topLevelJobConstructions.empty() &&
            topLevelJobConstructions.size() > 0);
  }

public:
  /// Get each top level job
  void ForEachTopLevelJobConstruction(
      std::function<void(const JobConstruction *construction)> callback);

  /// Get each top level job
  void ForEachTopLevelJob(std::function<void(const Job *job)> callback);
  /// Get each top level job
  void ForEachTopLevelExternalJob(std::function<void(const Job *job)> callback);

public:
};

} // namespace stone

#endif