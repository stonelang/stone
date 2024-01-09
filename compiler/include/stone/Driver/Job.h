#ifndef STONE_DRIVER_DRIVER_JOB_H
#define STONE_DRIVER_DRIVER_JOB_H

#include "stone/Basic/OptionSet.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/JobConstruction.h"

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
class Driver;
class JobConstruction;
class Compilation;

using JobList = llvm::ArrayRef<const Job *>;
using JobConstructionList = llvm::ArrayRef<const JobConstruction *>;

struct CommandInputPair {
  /// A filename provided from the user, either on the command line or in an
  /// input file map. Feeds into a Job graph, from InputActions, and is
  /// _associated_ with a PrimaryInput for a given Job, but may be upstream of
  /// the Job (and its PrimaryInput) and thus not necessarily passed as a
  /// filename to the job. Used as a key into the user-provided OutputFileMap
  /// (of BaseInputs and BaseOutputs), and used to derive downstream names --
  /// both temporaries and auxiliaries -- but _not_ used as a key into the
  /// DerivedOutputFileMap.
  llvm::StringRef Base;

  /// A filename that _will be passed_ to the command as a designated primary
  /// input. Typically either equal to BaseInput or a temporary with a name
  /// derived from the BaseInput it is related to. Also used as a key into
  /// the DerivedOutputFileMap.
  llvm::StringRef Primary;

  /// Construct a JobInputPair from a Base Input and, optionally, a Primary;
  /// if the Primary is empty, use the Base value for it.
  explicit CommandInputPair(llvm::StringRef BaseInput,
                            llvm::StringRef PrimaryInput)
      : Base(BaseInput),
        Primary(PrimaryInput.empty() ? BaseInput : PrimaryInput) {}
};

class CommandOutput final {

public:
  /// A CommandOutput designates one type of output as primary, though there
  /// may be multiple outputs of that type.
  file::FileType primaryOutputFileType;

  /// A CommandOutput also restricts its attention regarding additional-outputs
  /// to a subset of the PrimaryOutputs associated with its PrimaryInputs;
  /// sometimes multiple commands operate on the same PrimaryInput, in different
  /// phases (eg. autolink-extract and link both operate on the same .o file),
  /// so Jobs cannot _just_ rely on the presence of a primary output in the
  /// DerivedOutputFileMap.
  llvm::SmallSet<file::FileType, 4> additionalOutputFileTypes;

public:
  CommandOutput(file::FileType PrimaryOutputFileType);
};

class JobInfo final : public DriverAllocation<JobInfo> {
  friend JobConstruction;

  const JobConstruction *jc = nullptr;
  Compilation &compilation;

public:
  /// Dependency jobs for the main job
  llvm::SmallVector<const Job *> deps;

  /// You may just need compilation entities
  llvm::SmallVector<const CompilationEntity *> inputs;

  /// The command output for the job
  std::unique_ptr<CommandOutput> commandOutput;

public:
  JobInfo(const JobInfo &) = delete;
  void operator=(const JobInfo &) = delete;
  JobInfo(JobInfo &&) = delete;
  void operator=(JobInfo &&) = delete;

public:
  explicit JobInfo(const JobConstruction *jc,
                   Compilation &compilation) : jc(jc), compilation(compilation) {
    assert(jc != nullptr);
  }

  ~JobInfo() = default;

public:
  const JobConstruction *GetJobConstruction() const { return jc; }
  Compilation &GetCompilation() { return compilation; }
  const CommandOutput &GetCommandOutput() const { return *commandOutput; }

public:
  static JobInfo *Create(Driver &driver, const JobConstruction *jc);
};

class JobContext final {
  friend JobConstruction;

private:
  Compilation &compilation;

public:
  llvm::ArrayRef<const Job *> deps;
  llvm::ArrayRef<const CompilationEntity *> inputs;

  const CommandOutput &commandOutput;

public:
  JobContext(Compilation &compilation, llvm::ArrayRef<const Job *> deps,
             llvm::ArrayRef<const CompilationEntity *> inputs,
             const CommandOutput &commandOutput);
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

class Job : public TopLevelCompilationEntity {

  const JobConstruction &constructor;

public:
  using EnvironmentVector = std::vector<std::pair<const char *, const char *>>;
  /// If positive, contains llvm::ProcessID for a real Job on the host OS. If
  /// negative, contains a quasi-PID, which identifies a Job that's a member of
  /// a BatchJob _without_ denoting an operating system process.
  using JobProcessID = int64_t;
  enum class JobFlags : uint8_t {
    None = 1 << 0,
    TopLevel = 1 << 1,
  };
  /// Options that control the JobConstruction
  using JobOptions = stone::OptionSet<JobFlags>;
  JobOptions jobOptions;

protected:
  Job(CompilationEntityKind kind, const JobConstruction &constructor);
  Job(CompilationEntityKind kind, const JobConstruction &constructor,
      CompilationEntityList inputs);

public:
  const JobConstruction &GetConstructor() { return constructor; }

public:
  static Job *Create(const Driver &driver, const JobConstruction &constructor,
                     CompilationEntityList inputs);
};

class BatchJob final : public Job {

protected:
  BatchJob(const JobConstruction &constructor);
  BatchJob(const JobConstruction &constructor, CompilationEntityList inputs);
};

} // namespace stone
#endif