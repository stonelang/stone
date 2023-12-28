#include "stone/Driver/Job.h"
#include "stone/Driver/Compilation.h"

using namespace stone;

JobContext::JobContext(
    Compilation &compilation, llvm::ArrayRef<const Job *> inputs,
    llvm::ArrayRef<const JobConstruction *> inputConstructions,
    const JobOutput &jobOutput, const DriverOptions &driverOpts)
    : compilation(compilation), inputs(inputs),
      inputConstructions(inputConstructions), jobOutput(jobOutput),
      driverOpts(driverOpts) {}
