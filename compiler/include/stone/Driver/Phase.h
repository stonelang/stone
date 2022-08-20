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
class Phase;

enum class PhaseKind : uint8_t {
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

using PhaseInput = llvm::PointerUnion<stone::file::File *, Phase *>;
using PhaseInputList = llvm::ArrayRef<PhaseInput>;

class Phase {
  friend class Driver;

  PhaseKind kind;
  const Tool &tool;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<PhaseInput> inputs;

  const char *GetNameByKind(PhaseKind kind) const;
  /// Itents must be created through Driver::MakePhase(...)
  void *operator new(size_t size) { return ::operator new(size); };

public:
  using size_type = llvm::ArrayRef<PhaseInput>::size_type;
  using iterator = llvm::ArrayRef<PhaseInput>::iterator;
  using const_iterator = llvm::ArrayRef<PhaseInput>::const_iterator;

public:
  Phase() = delete;
  Phase(PhaseKind kind, const Tool &tool, PhaseInputList inputs,
        file::Type outputFileType);

  virtual ~Phase();

public:
  const char *GetName() const { return Phase::GetNameByKind(kind); }
  PhaseInputList GetInputs() { return inputs; }
  PhaseKind GetKind() const { return kind; }
  void AddInput(PhaseInput input) { inputs.push_back(input); }

public:
  /// Perform a complete dump of this action.
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
  static bool classof(const Phase *action) {
    return (action->GetKind() >= PhaseKind::First &&
            action->GetKind() <= PhaseKind::Last);
  }
};

class CompilePhase final : public Phase {
  PhaseInput primaryInput;

public:
  CompilePhase(const Tool &tool, file::Type outputFileType);
  CompilePhase(const Tool &tool, PhaseInput input, file::Type outputFileType);

public:
  PhaseInput GetPrimaryInput() { return primaryInput; }
  // void SetPrimaryInput(PhaseInput input) { primaryInput = input; }

public:
  static bool classof(const Phase *action) {
    return action->GetKind() == PhaseKind::Compile;
  }
};

class DynamicLinkPhase final : public Phase {
  bool withLTO;

public:
  DynamicLinkPhase(const Tool &tool, PhaseInputList inputs,
                   bool withLTO = false);

  bool WithLTO() { return withLTO; }

public:
  static bool classof(const Phase *action) {
    return action->GetKind() == PhaseKind::DynamicLink;
  }
};
class StaticLinkPhase final : public Phase {
public:
  StaticLinkPhase(const Tool &tool, PhaseInputList inputs);

public:
  static bool classof(const Phase *action) {
    return action->GetKind() == PhaseKind::StaticLink;
  }
};

class ExecutableLinkPhase final : public Phase {
public:
  ExecutableLinkPhase(const Tool &tool, PhaseInputList inputs);

public:
  static bool classof(const Phase *action) {
    return action->GetKind() == PhaseKind::ExecutableLink;
  }
};

class PhaseCache final {
public:
  /// We keep track of the Phase(s) that we are building for the module. Ex:
  /// CompilePhase
  llvm::SmallVector<PhaseInput, 16> forCompile;

  llvm::SmallVector<PhaseInput, 16> forLink;

  /// These are the top-level actions -- we use them recursively to build the
  /// jobs
  llvm::SmallVector<PhaseInput, 16> forTopLevel;

public:
  bool HasCompile() { return forCompile.size(); }
  void CacheForCompile(PhaseInput input) { forCompile.push_back(input); }

  // TODO: The approach that you are taking, this is not needed.
  // bool ForLink() { return forLink.size(); }
  // void CacheForLink(PhaseInput input) { forLink.push_back(input); }

  bool HasTopLevel() { return forTopLevel.size(); }
  void CacheForTopLevel(PhaseInput input) { forTopLevel.push_back(input); }
};

} // namespace stone

#endif
