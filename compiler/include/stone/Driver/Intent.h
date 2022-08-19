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
class Intent;

enum class IntentKind {
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
namespace intent {
using Input = llvm::PointerUnion<stone::file::File *, Intent *>;
using InputList = llvm::ArrayRef<intent::Input>;
} // namespace intent

class Intent {

  friend class Driver;

  IntentKind kind;
  const Tool &tool;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<intent::Input> inputs;

  const char *GetNameByKind(IntentKind kind) const;
  /// Itents must be created through Driver::MakeIntent(...)
  void *operator new(size_t size) { return ::operator new(size); };

public:
  using size_type = llvm::ArrayRef<intent::Input>::size_type;
  using iterator = llvm::ArrayRef<intent::Input>::iterator;
  using const_iterator = llvm::ArrayRef<intent::Input>::const_iterator;

public:
  Intent() = delete;
  Intent(IntentKind kind, const Tool &tool, intent::InputList inputs,
         file::Type outputFileType);

  virtual ~Intent();

public:
  const char *GetName() const { return Intent::GetNameByKind(kind); }
  intent::InputList GetInputs() { return inputs; }
  IntentKind GetKind() const { return kind; }
  void AddInput(intent::Input input) { inputs.push_back(input); }

public:
  /// Perform a complete dump of this intent.
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
  static bool classof(const Intent *intent) {
    return (intent->GetKind() >= IntentKind::First &&
            intent->GetKind() <= IntentKind::Last);
  }
};

class CompileIntent final : public Intent {
  intent::Input primaryInput;

public:
  CompileIntent(const Tool &tool, file::Type outputFileType);
  CompileIntent(const Tool &tool, intent::Input input,
                file::Type outputFileType);

public:
  intent::Input GetPrimaryInput() { return primaryInput; }
  // void SetPrimaryInput(intent::Input input) { primaryInput = input; }

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::Compile;
  }
};

class DynamicLinkIntent final : public Intent {
  bool withLTO;

public:
  DynamicLinkIntent(const Tool &tool, intent::InputList inputs,
                    bool withLTO = false);

  bool WithLTO() { return withLTO; }

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::DynamicLink;
  }
};
class StaticLinkIntent final : public Intent {
public:
  StaticLinkIntent(const Tool &tool, intent::InputList inputs);

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::StaticLink;
  }
};

class ExecutableLinkIntent final : public Intent {
public:
  ExecutableLinkIntent(const Tool &tool, intent::InputList inputs);

public:
  static bool classof(const Intent *intent) {
    return intent->GetKind() == IntentKind::ExecutableLink;
  }
};

class IntentCache final {
public:
  /// We keep track of the intents for the module that we are building.
  /// These are CompileIntent
  llvm::SmallVector<intent::Input, 16> forCompile;

  /// When are building the Intents(s), keep track of the linker dependecies
  llvm::SmallVector<intent::Input, 16> forLink;

  /// These are the top-level intents -- we use them recursively to build
  llvm::SmallVector<intent::Input, 16> forTop;

public:
  bool ForCompile() { return forCompile.size(); }
  void CacheForCompile(intent::Input input) { forCompile.push_back(input); }

  // TODO: The approach that you are taking, this is not needed.
  bool ForLink() { return forLink.size(); }
  void CacheForLink(intent::Input input) { forLink.push_back(input); }

  bool ForTop() { return forTop.size(); }
  void CacheForTop(intent::Input input) { forTop.push_back(input); }
};

} // namespace stone

#endif
