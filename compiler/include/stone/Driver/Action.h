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
class Action;

enum class ActionKind : uint8_t {
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
namespace action {
using Input = llvm::PointerUnion<stone::file::File *, Action *>;
using InputList = llvm::ArrayRef<action::Input>;
} // namespace action

class Action {
  friend class Driver;

  ActionKind kind;
  const Tool &tool;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<action::Input> inputs;

  const char *GetNameByKind(ActionKind kind) const;
  /// Itents must be created through Driver::MakeAction(...)
  void *operator new(size_t size) { return ::operator new(size); };

public:
  using size_type = llvm::ArrayRef<action::Input>::size_type;
  using iterator = llvm::ArrayRef<action::Input>::iterator;
  using const_iterator = llvm::ArrayRef<action::Input>::const_iterator;

public:
  Action() = delete;
  Action(ActionKind kind, const Tool &tool, action::InputList inputs,
         file::Type outputFileType);

  virtual ~Action();

public:
  const char *GetName() const { return Action::GetNameByKind(kind); }
  action::InputList GetInputs() { return inputs; }
  ActionKind GetKind() const { return kind; }
  void AddInput(action::Input input) { inputs.push_back(input); }

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
  static bool classof(const Action *action) {
    return (action->GetKind() >= ActionKind::First &&
            action->GetKind() <= ActionKind::Last);
  }
};

class CompileAction final : public Action {
  action::Input primaryInput;

public:
  CompileAction(const Tool &tool, file::Type outputFileType);
  CompileAction(const Tool &tool, action::Input input,
                file::Type outputFileType);

public:
  action::Input GetPrimaryInput() { return primaryInput; }
  // void SetPrimaryInput(action::Input input) { primaryInput = input; }

public:
  static bool classof(const Action *action) {
    return action->GetKind() == ActionKind::Compile;
  }
};

class DynamicLinkAction final : public Action {
  bool withLTO;

public:
  DynamicLinkAction(const Tool &tool, action::InputList inputs,
                    bool withLTO = false);

  bool WithLTO() { return withLTO; }

public:
  static bool classof(const Action *action) {
    return action->GetKind() == ActionKind::DynamicLink;
  }
};
class StaticLinkAction final : public Action {
public:
  StaticLinkAction(const Tool &tool, action::InputList inputs);

public:
  static bool classof(const Action *action) {
    return action->GetKind() == ActionKind::StaticLink;
  }
};

class ExecutableLinkAction final : public Action {
public:
  ExecutableLinkAction(const Tool &tool, action::InputList inputs);

public:
  static bool classof(const Action *action) {
    return action->GetKind() == ActionKind::ExecutableLink;
  }
};

class ActionCache final {
public:
  /// We keep track of the Action(s) that we are building for the module. Ex:
  /// CompileAction
  llvm::SmallVector<action::Input, 16> forCompile;

  llvm::SmallVector<action::Input, 16> forLink;

  /// These are the top-level actions -- we use them recursively to build the
  /// jobs
  llvm::SmallVector<action::Input, 16> forTopLevel;

public:
  bool HasCompile() { return forCompile.size(); }
  void CacheForCompile(action::Input input) { forCompile.push_back(input); }

  // TODO: The approach that you are taking, this is not needed.
  // bool ForLink() { return forLink.size(); }
  // void CacheForLink(action::Input input) { forLink.push_back(input); }

  bool HasTopLevel() { return forTopLevel.size(); }
  void CacheForTopLevel(action::Input input) { forTopLevel.push_back(input); }
};

} // namespace stone

#endif
