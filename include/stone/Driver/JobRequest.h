#ifndef STONE_DRIVER_JOBREQUEST_H
#define STONE_DRIVER_JOBREQUEST_H

#include "stone/Core/Color.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/Command.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Intent.h"
#include "stone/Driver/JobKind.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"

namespace stone {

enum class JobRequestKind : uint8_t {
  None = 0,
  Compile,
  Backend,
  MergeModule,
  DynamicLink,
  StaticLink,
  Assemble,
  ExecLink,
  First = Compile,
  Last = ExecLink,
};

using JobRequestInputs = llvm::ArrayRef<const file::File *>;
class JobRequest {

  JobRequestKind kind;
  llvm::TinyPtrVector<const file::File *> inputs;
  // file::Type outputFileType = file::Type::None;

public:
  JobRequest(JobRequestKind kind, JobRequestInputs inputs)
      : kind(kind), inputs(inputs) {}

public:
  /// Perform a complete dump of this job.
  virtual void Print(ColorOutputStream &stream,
                     llvm::StringRef terminator = "\n");

public:
  llvm::ArrayRef<const file::File *> GetInputs() { return inputs; }
  JobRequestKind GetKind() const { return kind; }
  void AddInput(const file::File *input) { inputs.push_back(input); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const JobRequest *job) {
    return (job->GetKind() >= JobRequestKind::First &&
            job->GetKind() <= JobRequestKind::Last);
  }

public:
  template <typename J, typename... Args>
  static std::unique_ptr<J> Make(Args &&...args) {
    return std::make_unique<J>(std::forward<Args>(args)...);
  }
};

class CompileJobRequest final : public JobRequest {
public:
  CompileJobRequest(JobRequestInputs inputs)
      : JobRequest(JobRequestKind::Compile, inputs) {}

public:
  static bool classof(const JobRequest *job) {
    return job->GetKind() == JobRequestKind::Compile;
  }
};

// These jobs have no parents -- they can directly receive inputs or other jobs
using JobRequestDeps = llvm::ArrayRef<const JobRequest *>;
class TopLevelJobRequest : public JobRequest {
  llvm::TinyPtrVector<const JobRequest *> deps;

public:
  using size_type = llvm::ArrayRef<const JobRequest *>::size_type;
  using iterator = llvm::ArrayRef<const JobRequest *>::iterator;
  using const_iterator = llvm::ArrayRef<const JobRequest *>::const_iterator;

public:
  TopLevelJobRequest(JobRequestKind kind, JobRequestInputs inputs)
      : JobRequest(kind, inputs) {}

  TopLevelJobRequest(JobRequestKind kind, JobRequestDeps deps)
      : JobRequest(kind, {}), deps(deps) {}

public:
  void AddDep(const JobRequest *dep) { deps.push_back(dep); }

  /// Perform a complete dump of this job.
  void Print(ColorOutputStream &stream,
             llvm::StringRef terminator = "\n") override;

public:
  size_type size() const { return deps.size(); }
  iterator begin() { return deps.begin(); }
  iterator end() { return deps.end(); }
  const_iterator begin() const { return deps.begin(); }
  const_iterator end() const { return deps.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const JobRequest *job) {
    return (job->GetKind() >= JobRequestKind::First &&
            job->GetKind() <= JobRequestKind::Last);
  }
};

// class DynamicLinkJobRequest final : public TopLevelJobRequest {
//   bool requiresLTO;

// public:
//   DynamicLinkJobRequest(const Tool &tool, Inputs inputs,
//                         bool requiresLTO = false)
//       : TopLevelJobRequest(JobRequestKind::DynamicLink, tool, inputs),
//         requiresLTO(requiresLTO) {}

//   DynamicLinkJobRequest(const Tool &tool, Deps deps, bool requiresLTO =
//   false)
//       : TopLevelJobRequest(JobRequestKind::DynamicLink, tool, deps),
//         requiresLTO(requiresLTO) {}

// public:
//   static bool classof(const JobRequest *job) {
//     return job->GetKind() == JobRequestKind::DynamicLink;
//   }
// };

// class StaticLinkJobRequest final : public TopLevelJobRequest {

// public:
//   StaticLinkJobRequest(const Tool &tool, Inputs inputs)
//       : TopLevelJobRequest(JobRequestKind::StaticLink, tool, inputs) {}

//   StaticLinkJobRequest(const Tool &tool, Deps deps)
//       : TopLevelJobRequest(JobRequestKind::StaticLink, tool, deps) {}

// public:
//   static bool classof(const JobRequest *job) {
//     return job->GetKind() == JobRequestKind::StaticLink;
//   }
// };

// class ExecLinkJobRequest final : public TopLevelJobRequest {

// public:
//   ExecLinkJobRequest(const Tool &tool, Inputs inputs)
//       : TopLevelJobRequest(JobRequestKind::ExecLink, tool, inputs) {}

//   ExecLinkJobRequest(const Tool &tool, Deps deps)
//       : TopLevelJobRequest(JobRequestKind::ExecLink, tool, deps) {}

// public:
//   static bool classof(const JobRequest *job) {
//     return job->GetKind() == JobRequestKind::ExecLink;
//   }
// };

} // namespace stone
#endif