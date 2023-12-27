#ifndef STONE_COMPILER_ACTION_H
#define STONE_COMPILER_ACTION_H

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/DriverOptions.h"

#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"

namespace stone {
class Tool;
class JobAction;

enum class CompilationExecutionKind : uint8_t {
  Compile = 0,
  Backend,
  MergeModule,
  ModuleWrap,
  DynamicLink,
  StaticLink,
  ExecutableLink,
  Assemble,
  First = Compile,
  Last = Assemble
};

using JobActionInput = llvm::PointerUnion<stone::file::File *, JobAction *>;
using JobActionInputList = llvm::ArrayRef<JobActionInput>;

class JobAction {
  friend class Driver;

  JobActionKind kind;
  const Tool &tool;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<JobActionInput> inputs;

  const char *GetNameByKind(JobActionKind kind) const;
  /// Itents must be created through Driver::MakeJobAction(...)
  void *operator new(size_t size) { return ::operator new(size); };

public:
  using size_type = llvm::ArrayRef<JobActionInput>::size_type;
  using iterator = llvm::ArrayRef<JobActionInput>::iterator;
  using const_iterator = llvm::ArrayRef<JobActionInput>::const_iterator;

public:
  JobAction() = delete;
  JobAction(JobActionKind kind, const Tool &tool, JobActionInputList inputs,
            file::Type outputFileType);

  virtual ~JobAction();

public:
  const char *GetName() const { return JobAction::GetNameByKind(kind); }
  JobActionInputList GetInputs() { return inputs; }
  JobActionKind GetKind() const { return kind; }
  void AddInput(JobActionInput input) { inputs.push_back(input); }

public:
  /// Perform a complete dump of this action.
  virtual void Print(ColorStream &stream,
                     llvm::StringRef terminator = "\n") const;

public:
  size_type size() const { return inputs.size(); }
  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const JobAction *action) {
    return (action->GetKind() >= JobActionKind::First &&
            action->GetKind() <= JobActionKind::Last);
  }
};

class CompileJobAction final : public JobAction {
  JobActionInput primaryInput;

public:
  CompileJobAction(const Tool &tool, file::Type outputFileType);
  CompileJobAction(const Tool &tool, JobActionInput input,
                   file::Type outputFileType);

public:
  JobActionInput GetPrimaryInput() { return primaryInput; }
  // void SetPrimaryInput(JobActionInput input) { primaryInput = input; }

public:
  static bool classof(const JobAction *phase) {
    return phase->GetKind() == JobActionKind::Compile;
  }
};

class DynamicLinkJobAction final : public JobAction {
  bool withLTO;

public:
  DynamicLinkJobAction(const Tool &tool, JobActionInputList inputs,
                       bool withLTO = false);

  bool WithLTO() { return withLTO; }

public:
  static bool classof(const JobAction *phase) {
    return phase->GetKind() == JobActionKind::DynamicLink;
  }
};
class StaticLinkJobAction final : public JobAction {
public:
  StaticLinkJobAction(const Tool &tool, JobActionInputList inputs);

public:
  static bool classof(const JobAction *phase) {
    return phase->GetKind() == JobActionKind::StaticLink;
  }
};

class ExecutableLinkJobAction final : public JobAction {
public:
  ExecutableLinkJobAction(const Tool &tool, JobActionInputList inputs);

public:
  static bool classof(const JobAction *phase) {
    return phase->GetKind() == JobActionKind::ExecutableLink;
  }
};

class JobActionCache final {
public:
  /// We keep track of the JobAction(s) that we are building for the module. Ex:
  /// CompileJobAction
  llvm::SmallVector<JobActionInput, 16> forCompile;

  // llvm::SmallVector<JobActionInput, 16> forLink;

  /// These are the top-level actions -- we use them recursively to build the
  /// jobs
  llvm::SmallVector<JobActionInput, 16> forTopLevel;

public:
  bool HasCompile() { return forCompile.size(); }
  void CacheForCompile(JobActionInput input) { forCompile.push_back(input); }

  // TODO: The approach that you are taking, this is not needed.
  // bool ForLink() { return forLink.size(); }
  // void CacheForLink(JobActionInput input) { forLink.push_back(input); }

  // bool HasTopLevel() { return forTopLevel.size(); }
  // void CacheForTopLevel(JobActionInput input) { forTopLevel.push_back(input);
  // }
};

} // namespace stone

#endif
