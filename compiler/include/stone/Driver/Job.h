#ifndef STONE_DRIVER_DRIVER_JOB_H
#define STONE_DRIVER_DRIVER_JOB_H

#include "stone/Basic/File.h"
#include "stone/Driver/DriverOptions.h"

#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Job;
class Compilation;
class JobConstruction;


class JobOutput final {

public:
  /// A CommandOutput designates one type of output as primary, though there
  /// may be multiple outputs of that type.
  file::Type PrimaryOutputFileType;

  /// A CommandOutput also restricts its attention regarding additional-outputs
  /// to a subset of the PrimaryOutputs associated with its PrimaryInputs;
  /// sometimes multiple commands operate on the same PrimaryInput, in different
  /// phases (eg. autolink-extract and link both operate on the same .o file),
  /// so Jobs cannot _just_ rely on the presence of a primary output in the
  /// DerivedOutputFileMap.
  llvm::SmallSet<file::Type, 4> AdditionalOutputFileTypes;
};

class JobContext final {
private:
  Compilation &compilation;

public:
  llvm::ArrayRef<const Job *> inputs;
  llvm::ArrayRef<const JobConstruction *> inputConstructions;

  const JobOutput &jobOutput;
  const DriverOptions &driverOpts;

public:
  JobContext(Compilation &compilation, llvm::ArrayRef<const Job *> Inputs,
             llvm::ArrayRef<const JobConstruction *> inputConstructions,
             const JobOutput &jobOutput, const DriverOptions &driverOpts);
};

class Job {
public:
};

} // namespace stone
#endif