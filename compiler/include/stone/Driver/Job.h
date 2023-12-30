#ifndef STONE_DRIVER_DRIVER_JOB_H
#define STONE_DRIVER_DRIVER_JOB_H

#include "stone/Basic/File.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/DriverInvocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {
class Job;
class Compilation;
class JobConstruction;

struct JobInputPair {
  /// A filename provided from the user, either on the command line or in an
  /// input file map. Feeds into a Job graph, from InputActions, and is
  /// _associated_ with a PrimaryInput for a given Job, but may be upstream of
  /// the Job (and its PrimaryInput) and thus not necessarily passed as a
  /// filename to the job. Used as a key into the user-provided OutputFileMap
  /// (of BaseInputs and BaseOutputs), and used to derive downstream names --
  /// both temporaries and auxiliaries -- but _not_ used as a key into the
  /// DerivedOutputFileMap.
  StringRef Base;

  /// A filename that _will be passed_ to the command as a designated primary
  /// input. Typically either equal to BaseInput or a temporary with a name
  /// derived from the BaseInput it is related to. Also used as a key into
  /// the DerivedOutputFileMap.
  StringRef Primary;

  /// Construct a JobInputPair from a Base Input and, optionally, a Primary;
  /// if the Primary is empty, use the Base value for it.
  explicit JobInputPair(StringRef BaseInput, StringRef PrimaryInput)
      : Base(BaseInput),
        Primary(PrimaryInput.empty() ? BaseInput : PrimaryInput) {}
};

class JobOutput final {

public:
  /// A CommandOutput designates one type of output as primary, though there
  /// may be multiple outputs of that type.
  file::FileType PrimaryOutputFileType;

  /// A CommandOutput also restricts its attention regarding additional-outputs
  /// to a subset of the PrimaryOutputs associated with its PrimaryInputs;
  /// sometimes multiple commands operate on the same PrimaryInput, in different
  /// phases (eg. autolink-extract and link both operate on the same .o file),
  /// so Jobs cannot _just_ rely on the presence of a primary output in the
  /// DerivedOutputFileMap.
  llvm::SmallSet<file::FileType, 4> AdditionalOutputFileTypes;
};

class JobContext final {
private:
  Compilation &compilation;

public:
  llvm::ArrayRef<const Job *> inputs;
  llvm::ArrayRef<const JobConstruction *> inputConstructions;

  const JobOutput &jobOutput;

public:
  JobContext(Compilation &compilation, llvm::ArrayRef<const Job *> Inputs,
             llvm::ArrayRef<const JobConstruction *> inputConstructions,
             const JobOutput &jobOutput);
};

enum class JobCondition {
  // There was no information about the previous build (i.e., an input map),
  // or the map marked this Job as dirty or needing a cascading build.
  // Be maximally conservative with dependencies.
  Always,
  // The input changed, or this job was scheduled as non-cascading in the last
  // build but didn't get to run.
  RunWithoutCascading,
  // The best case: input didn't change, output exists.
  // Only run if it depends on some other thing that changed.
  CheckDependencies,
  // Run no matter what (but may or may not cascade).
  NewlyAdded
};

class Job /*: public DriverAllocation<Job>*/ {
public:
  using EnvironmentVector = std::vector<std::pair<const char *, const char *>>;
  /// If positive, contains llvm::ProcessID for a real Job on the host OS. If
  /// negative, contains a quasi-PID, which identifies a Job that's a member of
  /// a BatchJob _without_ denoting an operating system process.
  using JobProcessID = int64_t;

private:
  /// The action which caused the creation of this Job, and the conditions
  /// under which it must be run.
  llvm::PointerIntPair<const JobConstruction *, 2, JobCondition>
      constructionAndCondition;

  /// The list of other Jobs which are inputs to this Job.
  llvm::SmallVector<const Job *, 4> inputs;

  /// The output of this job;
  std::unique_ptr<JobOutput> jobOutput;

  /// The executable to run.
  const char *executable = nullptr;

  /// The list of program arguments (not including the implicit first argument,
  /// which will be the Executable).
  ///
  /// These argument strings must be kept alive as long as the Job is alive.
  llvm::opt::ArgStringList arguments;

  /// Additional variables to set in the process environment when running.
  ///
  /// These strings must be kept alive as long as the Job is alive.
  EnvironmentVector extraEnvironment;

  /// The modification time of the main input file, if any.
  llvm::sys::TimePoint<> inputModificationTime = llvm::sys::TimePoint<>::max();

public:
};

} // namespace stone
#endif