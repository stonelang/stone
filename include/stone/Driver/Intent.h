#ifndef STONE_DRIVER_INTENT_H
#define STONE_DRIVER_INTENT_H

#include "stone/Core/File.h"
#include "stone/Core/LLVM.h"
#include "stone/Driver/DriverOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Chrono.h"

namespace stone {

enum class IntentKind : unsigned {
  None = 0,
  Process,
  Compile,
  MergeModule,
  ExecutableLink,
  DynamicLink,
  StaticLink,
  GenPCH,
  FirstCompilationIntent = Compile,
  LastCompilationIntent = GenPCH,
};

enum class IntentLevel : unsigned {
  None = 0,
  Top,
  Dep,
};

class Intent {
  IntentKind kind;
  IntentLevel level;

public:
  using size_type = llvm::ArrayRef<const Intent *>::size_type;
  using iterator = llvm::ArrayRef<const Intent *>::iterator;
  using const_iterator = llvm::ArrayRef<const Intent *>::const_iterator;

public:
  Intent(IntentKind kind, IntentLevel level) : kind(kind), level(level) {}
  virtual ~Intent() = default;

public:
  IntentKind GetKind() const { return kind; }
  IntentLevel GetLevel() const { return level; }
  const char *GetName() const { return Intent::GetNameByKind(kind); }

public:
  static const char *GetNameByKind(IntentKind kind);
};

class ProcessIntent final : public Intent {
  const file::File &input;

public:
  ProcessIntent(const file::File &input)
      : Intent(IntentKind::Process, IntentLevel::None), input(input) {}

  const file::File &GetInput() const { return input; }

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::Process;
  }
};

using Intents = llvm::ArrayRef<const Intent *>;
class CompilationIntent : public Intent {

  llvm::TinyPtrVector<const Intent *> inputs;

public:
  CompilationIntent(IntentKind kind, IntentLevel level, Intents inputs)
      : Intent(kind, level), inputs(inputs) {}

public:
  llvm::ArrayRef<const Intent *> GetInputs() const { return inputs; }
  void AddInput(const Intent *input) { inputs.push_back(input); }

public:
  size_type size() const { return inputs.size(); }
  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const Intent *intent) {
    return (intent->GetKind() >= IntentKind::FirstCompilationIntent &&
            intent->GetKind() <= IntentKind::LastCompilationIntent);
  }
};

// Not valid for compile session
class CompileIntent final : public CompilationIntent {
public:
  CompileIntent(IntentLevel level, Intent *input)
      : CompilationIntent(IntentKind::Compile, level, input) {}

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::Compile;
  }
};

class LinkIntent : public CompilationIntent {
  LinkMode linkMode;

public:
  LinkIntent(IntentKind kind, Intents inputs)
      : CompilationIntent(kind, IntentLevel::Top, inputs) {}

public:
};

class StaticLinkIntent final : public LinkIntent {
public:
  StaticLinkIntent(Intents inputs)
      : LinkIntent(IntentKind::StaticLink, inputs) {}

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::StaticLink;
  }
};

class DynamicLinkIntent final : public LinkIntent {
  bool requiresLTO;

public:
  DynamicLinkIntent(Intents inputs, bool requiresLTO = false)
      : LinkIntent(IntentKind::DynamicLink, inputs), requiresLTO(requiresLTO) {}

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::DynamicLink;
  }
};

class ExecutableLinkIntent final : public LinkIntent {
public:
  ExecutableLinkIntent(Intents inputs)
      : LinkIntent(IntentKind::ExecutableLink, inputs) {}

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::ExecutableLink;
  }
};

class MergeModuleIntent final : public CompilationIntent {
public:
  MergeModuleIntent(Intents inputs)
      : CompilationIntent(IntentKind::MergeModule, IntentLevel::Top, inputs) {}

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::MergeModule;
  }
};

} // namespace stone

#endif
