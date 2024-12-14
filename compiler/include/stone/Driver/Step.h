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
class Step;
class Driver;

enum class StepKind : uint8_t {
  Input = 0,
  Compile,
  Backend,
  GeneratePCH,
  MergeModule,
  ModuleWrap,
  DynamicLink,
  StaticLink,
  Interpret,
  AutolinkExtract,
};

using Steps = llvm::SmallVector<const Step *, 3>;
constexpr size_t StepAlignInBits = 8;

class alignas(1 << StepAlignInBits) Step : public DriverAllocation<Step> {

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
  bool IsDynamicLink() const { return GetKind() == StepKind::DynamicLink; }
  bool IsStaticLink() const { return GetKind() == StepKind::StaticLink; }
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
enum class LinkMode : uint8_t {
  // We are not linking
  None = 0,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  Executable,

  // The default library output: 'stone test.stone -emit-library ->test.dylib'
  DynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  StaticLibrary
};
class DynamicLinkStep final : public JobStep {

  bool withLTO;
  LinkMode linkMode;

public:
  DynamicLinkStep(Steps inputs, LinkMode linkMode, bool withLTO = false);

public:
  bool WithLTO() const { return withLTO; }

public:
  static DynamicLinkStep *Create(Driver &driver, Steps inputs,
                                 LinkMode linkMode, bool withLTO = false);
  static bool classof(const Step *step) { return step->IsDynamicLink(); }
};
class StaticLinkStep final : public JobStep {
  LinkMode linkMode;

public:
  StaticLinkStep(Steps inputs, LinkMode linkMode);

public:
  LinkMode GetLinkMode() { return linkMode; }

public:
  static StaticLinkStep *Create(Driver &driver, Steps inputs,
                                LinkMode linkMode);
  static bool classof(const Step *step) { return step->IsStaticLink(); }
};

} // namespace stone

#endif