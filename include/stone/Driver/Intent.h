#ifndef STONE_DRIVER_INTENT_H
#define STONE_DRIVER_INTENT_H

#include "stone/Core/File.h"
#include "stone/Core/LLVM.h"

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
};

class Intent {
  IntentKind kind;

public:
  Intent(IntentKind kind) : kind(kind) {}
  virtual ~Intent() = default;

public:
  IntentKind GetKind() const { return kind; }
  const char *GetName() const { return Intent::GetNameByKind(kind); }

public:
  static const char *GetNameByKind(IntentKind kind);
};

class ProcessIntent final : public Intent {
  const file::File &input;

public:
  ProcessIntent(const file::File &input)
      : Intent(IntentKind::Process), input(input) {}

  const file::File &GetInput() const { return input; }

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::Process;
  }
};

using Intents = llvm::ArrayRef<const Intent *>;
class CompilationIntent : public Intent {

  bool topLevel;
  llvm::TinyPtrVector<const Intent *> inputs;

public:
  CompilationIntent(IntentKind kind, Intents inputs, bool topLevel)
      : Intent(kind), inputs(inputs), topLevel(topLevel) {}

public:
  bool IsTopLevel() { return topLevel; }
};

// Not valid for compile session
class CompileIntent final : public CompilationIntent {
public:
  CompileIntent(Intent *input, bool topLevel)
      : CompilationIntent(IntentKind::Compile, input,
                          topLevel /*Sometimes TopLevel*/) {}

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::Compile;
  }
};

class LinkIntent : public CompilationIntent {
  /// LinkMode
public:
  LinkIntent(IntentKind kind, Intents inputs)
      : CompilationIntent(kind, inputs, true /*TopLevel*/) {}

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
public:
  DynamicLinkIntent(Intents inputs, bool topLevel)
      : LinkIntent(IntentKind::DynamicLink, inputs) {}

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
      : CompilationIntent(IntentKind::MergeModule, inputs, true /*TopLevel*/) {}

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::MergeModule;
  }
};

} // namespace stone

#endif
