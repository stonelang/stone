#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Basic/OptionSet.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Support/InputFile.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
class Action;
class Driver;

enum class ActionKind : uint8_t {
  Input = 0,
  CompileJob,
  BackendJob,
  GeneratePCHJob,
  MergeModuleJob,
  ModuleWrapJob,
  DynamicLinkJob,
  StaticLinkJob,
  InterpretJob,
  AutolinkExtractJob,
  FirstJob = CompileJob,
  LastJob = AutolinkExtractJob
};

/// A list of all job construction inputs
using ActionList = llvm::SmallVector<const Action *, 3>;

constexpr size_t ActionAlignInBits = 8;

class alignas(1 << ActionAlignInBits) Action : public DriverAllocation<Action> {

  ActionKind kind;
  FileType fileType;
  ActionList allInputs;

public:
  using size_type = ActionList::size_type;
  using input_iterator = ActionList::iterator;
  using input_const_iterator = ActionList::const_iterator;
  using input_range = llvm::iterator_range<input_iterator>;
  using input_const_range = llvm::iterator_range<input_const_iterator>;

public:
  Action(ActionKind kind, FileType fileType)
      : Action(kind, ActionList(), fileType) {}

  Action(ActionKind kind, Action *input, FileType fileType)
      : Action(kind, ActionList({input}), fileType) {}

  Action(ActionKind kind, Action *input)
      : Action(kind, ActionList({input}), input->GetFileType()) {}

  Action(ActionKind kind, const ActionList &inputs, FileType fileType)
      : kind(kind), fileType(fileType), allInputs(inputs) {}

public:
  ActionKind GetKind() const { return kind; }
  FileType GetFileType() const { return fileType; }
  bool HasFileType() { return fileType != FileType::None; }

  ActionList &GetInputs() { return allInputs; }
  const ActionList &GetInputs() const { return allInputs; }
  void AddInput(Action *input) { allInputs.push_back(input); }

public:
  size_type size() const { return allInputs.size(); }
  input_iterator input_begin() { return allInputs.begin(); }
  input_iterator input_end() { return allInputs.end(); }
  input_range inputs() { return input_range(input_begin(), input_end()); }
  input_const_iterator input_begin() const { return allInputs.begin(); }
  input_const_iterator input_end() const { return allInputs.end(); }
  input_const_range inputs() const {
    return input_const_range(input_begin(), input_end());
  }

public:
  bool IsInput() const { return GetKind() == ActionKind::Input; }

  bool IsCompileJob() const { return GetKind() == ActionKind::CompileJob; }
  bool IsBackendJob() const { return GetKind() == ActionKind::BackendJob; }
  bool IsGeneratePCHJob() const {
    return GetKind() == ActionKind::GeneratePCHJob;
  }
  bool IsMergeModuleJob() const {
    return GetKind() == ActionKind::MergeModuleJob;
  }
  bool IsModuleWrapJob() const {
    return GetKind() == ActionKind::ModuleWrapJob;
  }
  bool IsDynamicLinkJob() const {
    return GetKind() == ActionKind::DynamicLinkJob;
  }
  bool IsStaticLinkJob() const {
    return GetKind() == ActionKind::StaticLinkJob;
  }
  bool IsInterpretJob() const { return GetKind() == ActionKind::InterpretJob; }
  bool IsAutolinkExtractJob() const {
    return GetKind() == ActionKind::AutolinkExtractJob;
  }
};

class InputAction final : public Action {
  InputFile input;

public:
  InputAction(InputFile input);

public:
  InputFile GetInput() { return input; }

public:
  static InputAction *Create(Driver &driver, InputFile input);

public:
  static bool classof(const Action *action) { return action->IsInput(); }
};

class JobAction : public Action {

protected:
  JobAction(ActionKind kind, Action *input, FileType fileType);
  JobAction(ActionKind kind, const ActionList &inputs, FileType fileType);

public:
  static bool classof(const Action *action) {
    return (action->GetKind() >= ActionKind::FirstJob &&
            action->GetKind() <= ActionKind::LastJob);
  }
};

class CompileJobAction : public JobAction {

public:
  /// In this scenario, one compile job with all inputs to be added.
  CompileJobAction(FileType outputFileType);

  /// In this scenario, one compile job for eache input.
  CompileJobAction(Action *input, FileType outputFileType);

public:
  static CompileJobAction *Create(Driver &driver, FileType outputFileType);
  static CompileJobAction *Create(Driver &driver, Action *input,
                                  FileType outputFileType);

public:
  static bool classof(const Action *action) { return action->IsCompileJob(); }
};

class BackendJobAction : public JobAction {

  // For multi-threaded compilations, the CompileJobAction produces multiple
  // output bitcode-files. For each bitcode-file a BackendJobAction is created.
  // This index specifies which of the files to select for the input.
  size_t inputIndex;

public:
  BackendJobAction(Action *input, FileType outputFileType, size_t inputIndex);

public:
  static BackendJobAction *Create(Driver &driver, Action *input,
                                  FileType outputFileType, size_t inputIndex);
  static bool classof(const Action *action) { return action->IsBackendJob(); }
};
class MergeModuleJobAction : public JobAction {

public:
  MergeModuleJobAction(ActionList inputs);

public:
  static MergeModuleJobAction *Create(Driver &driver, ActionList inputs);
  static bool classof(const Action *action) {
    return action->IsMergeModuleJob();
  }
};
enum class LinkMode : uint8_t {
  // We are not linking
  None = 0,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  Executable,

  // The default library output: 'stone test.stone -emit-library -> test.dylib'
  DynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  StaticLibrary
};
class DynamicLinkJobAction final : public JobAction {

  bool withLTO;
  LinkMode linkMode;

public:
  DynamicLinkJobAction(ActionList inputs, LinkMode linkMode,
                       bool withLTO = false);

public:
  bool WithLTO() const { return withLTO; }

public:
  static DynamicLinkJobAction *Create(Driver &driver, ActionList inputs,
                                      LinkMode linkMode, bool withLTO = false);
  static bool classof(const Action *action) {
    return action->IsDynamicLinkJob();
  }
};
class StaticLinkJobAction final : public JobAction {
  LinkMode linkMode;

public:
  StaticLinkJobAction(ActionList inputs, LinkMode linkMode);

public:
  LinkMode GetLinkMode() { return linkMode; }

public:
  static StaticLinkJobAction *Create(Driver &driver, ActionList inputs,
                                     LinkMode linkMode);
  static bool classof(const Action *action) {
    return action->IsStaticLinkJob();
  }
};

} // namespace stone

#endif