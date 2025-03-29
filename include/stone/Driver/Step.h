#ifndef STONE_DRIVER_DRIVER_STEP_H
#define STONE_DRIVER_DRIVER_STEP_H

#include "stone/Basic/Allocation.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/StepKind.h"
#include "stone/Support/InputFile.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
class Step;
class Driver;

using Steps = llvm::SmallVector<const Step *, 3>;
constexpr size_t StepAlignInBits = 8;

class alignas(1 << StepAlignInBits) Step : public Allocation<Step> {

  StepKind kind;
  FileType fileType;
  Steps allInputs;

public:
  using size_type = Steps::size_type;
  using input_iterator = Steps::iterator;
  using input_const_iterator = Steps::const_iterator;
  using input_range = llvm::iterator_range<input_iterator>;
  using input_const_range = llvm::iterator_range<input_const_iterator>;

public:
  Step(StepKind kind, FileType fileType);
  Step(StepKind kind, Step *input, FileType fileType);
  Step(StepKind kind, Step *input);
  Step(StepKind kind, const Steps &inputs, FileType fileType);

public:
  StepKind GetKind() const { return kind; }
  FileType GetFileType() const { return fileType; }
  bool HasFileType() { return fileType != FileType::None; }

  Steps &GetInputs() { return allInputs; }
  const Steps &GetInputs() const { return allInputs; }
  void AddInput(Step *input) { allInputs.push_back(input); }

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
  bool IsInput() const { return GetKind() == StepKind::Input; }
  bool IsCompile() const { return GetKind() == StepKind::Compile; }
  bool IsBackend() const { return GetKind() == StepKind::Backend; }
  bool IsGeneratePCH() const { return GetKind() == StepKind::GeneratePCH; }
  bool IsMergeModule() const { return GetKind() == StepKind::MergeModule; }
  bool IsModuleWrap() const { return GetKind() == StepKind::ModuleWrap; }
  bool IsLink() const { return GetKind() == StepKind::Link; }

  bool IsInterpret() const { return GetKind() == StepKind::Interpret; }
  bool IsAutolinkExtract() const {
    return GetKind() == StepKind::AutolinkExtract;
  }
  bool IsJobStep() const { return Step::IsJobStep(GetKind()); }

public:
  static bool IsJobStep(StepKind kind);
};

class InputStep final : public Step {
  InputFile input;

public:
  InputStep(InputFile input);

public:
  InputFile GetInput() { return input; }

public:
  static InputStep *Create(Driver &driver, InputFile input);

public:
  static bool classof(const Step *step) { return step->IsInput(); }
};

class JobStep : public Step {

protected:
  JobStep(StepKind kind, Step *input, FileType fileType);
  JobStep(StepKind kind, const Steps &inputs, FileType fileType);

public:
  static bool classof(const Step *step) { return step->IsJobStep(); }
};

class CompileStep final : public JobStep {

public:
  /// In this scenario, one compile job with all inputs to be added.
  CompileStep(FileType outputFileType);

  /// In this scenario, one compile job for eache input.
  CompileStep(Step *input, FileType outputFileType);

public:
  static CompileStep *Create(Driver &driver, FileType outputFileType);
  static CompileStep *Create(Driver &driver, Step *input,
                             FileType outputFileType);

public:
  static bool classof(const Step *step) { return step->IsCompile(); }
};

class BackendStep : public JobStep {

  // For multi-threaded compilations, the CompileStep produces multiple
  // output bitcode-files. For each bitcode-file a BackendStep is created.
  // This index specifies which of the files to select for the input.
  size_t inputIndex;

public:
  BackendStep(Step *input, FileType outputFileType, size_t inputIndex);

public:
  static BackendStep *Create(Driver &driver, Step *input,
                             FileType outputFileType, size_t inputIndex);
  static bool classof(const Step *step) { return step->IsBackend(); }
};

class MergeModuleStep : public JobStep {

public:
  MergeModuleStep(Steps inputs);

public:
  static MergeModuleStep *Create(Driver &driver, Steps inputs);
  static bool classof(const Step *step) { return step->IsMergeModule(); }
};

class LinkStep final : public JobStep {

  bool allowLTO;
  LinkType linkType;

public:
  LinkStep(Steps inputs, LinkType linkType, bool allowLTO = false);

public:
  bool AllowLTO() const { return allowLTO; }

  LinkType GetLinkType() const { return linkType; }

  bool IsDynamicLibrary() const {
    return GetLinkType() == LinkType::DynamicLibrary;
  }
  bool IsStaticLibrary() const {
    return GetLinkType() == LinkType::StaticLibrary;
  }
  bool IsExecutable() const { return GetLinkType() == LinkType::Executable; }

public:
  static LinkStep *Create(Driver &driver, Steps inputs, LinkType linkType,
                          bool allowLTO = false);

  static bool classof(const Step *step) { return step->IsLink(); }
};

} // namespace stone

#endif