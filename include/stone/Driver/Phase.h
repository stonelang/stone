#ifndef STONE_COMPILER_INTENT_H
#define STONE_COMPILER_INTENT_H

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
class Phase;

enum class PhaseKind {
  Compile = 0,
  Backend,
  MergeModule,
  DynamicLink,
  StaticLink,
  ExecutableLink,
  Assemble,
  First = Compile,
  Last = Assemble
};
namespace phase {
using Input = llvm::PointerUnion<stone::file::File *, Phase *>;
using InputList = llvm::ArrayRef<phase::Input>;
} // namespace phase

class Phase {

  PhaseKind kind;
  const Tool &tool;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<phase::Input> inputs;

  const char *GetNameByKind(PhaseKind kind) const;

  friend class Driver;
  /// Itents must be created through Driver::MakePhase(...)
  void *operator new(size_t size) { return ::operator new(size); };

public:
  using size_type = llvm::ArrayRef<phase::Input>::size_type;
  using iterator = llvm::ArrayRef<phase::Input>::iterator;
  using const_iterator = llvm::ArrayRef<phase::Input>::const_iterator;

public:
  Phase() = delete;
  Phase(PhaseKind kind, const Tool &tool, phase::InputList inputs,
        file::Type outputFileType);
  virtual ~Phase();

public:
  const char *GetName() const { return Phase::GetNameByKind(kind); }
  phase::InputList GetInputs() { return inputs; }
  PhaseKind GetKind() const { return kind; }
  void AddInput(phase::Input input) { inputs.push_back(input); }

public:
  /// Perform a complete dump of this phase.
  virtual void Print(ColorfulStream &stream,
                     llvm::StringRef terminator = "\n") const;

public:
  size_type size() const { return inputs.size(); }
  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const Phase *phase) {
    return (phase->GetKind() >= PhaseKind::First &&
            phase->GetKind() <= PhaseKind::Last);
  }
};

class CompilePhase final : public Phase {
  phase::Input primaryInput;

public:
  CompilePhase(const Tool &tool, file::Type outputFileType);
  CompilePhase(const Tool &tool, phase::Input input, file::Type outputFileType);

public:
  phase::Input GetPrimaryInput() { return primaryInput; }
  // void SetPrimaryInput(phase::Input input) { primaryInput = input; }

public:
  static bool classof(const Phase *phase) {
    return phase->GetKind() == PhaseKind::Compile;
  }
};

class DynamicLinkPhase final : public Phase {
  bool withLTO;

public:
  DynamicLinkPhase(const Tool &tool, phase::InputList inputs,
                   bool withLTO = false);

  bool WithLTO() { return withLTO; }

public:
  static bool classof(const Phase *phase) {
    return phase->GetKind() == PhaseKind::DynamicLink;
  }
};
class StaticLinkPhase final : public Phase {
public:
  StaticLinkPhase(const Tool &tool, phase::InputList inputs);

public:
  static bool classof(const Phase *phase) {
    return phase->GetKind() == PhaseKind::StaticLink;
  }
};

class ExecutableLinkPhase final : public Phase {
public:
  ExecutableLinkPhase(const Tool &tool, phase::InputList inputs);

public:
  static bool classof(const Phase *phase) {
    return phase->GetKind() == PhaseKind::ExecutableLink;
  }
};

class PhaseCache final {
public:
  /// We keep track of the phases for the module that we are building.
  /// These are CompilePhase
  llvm::SmallVector<phase::Input, 16> forCompile;

  /// When are building the Phases(s), keep track of the linker dependecies
  llvm::SmallVector<phase::Input, 16> forLink;

  /// These are the top-level phases -- we use them recursively to build
  llvm::SmallVector<phase::Input, 16> forTop;

public:
  bool ForCompile() { return forCompile.size(); }
  void CacheForCompile(phase::Input input) { forCompile.push_back(input); }

  // TODO: The approach that you are taking, this is not needed.
  bool ForLink() { return forLink.size(); }
  void CacheForLink(phase::Input input) { forLink.push_back(input); }

  bool ForTop() { return forTop.size(); }
  void CacheForTop(phase::Input input) { forTop.push_back(input); }
};

} // namespace stone

#endif
