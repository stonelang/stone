#ifndef STONE_COMPILATION_COMPILATIONUNIT_H
#define STONE_COMPILATION_COMPILATIONUNIT_H

#include "stone/Core/Color.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/Command.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/DriverOptions.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"

namespace stone {

class Tool;
class ToolChain;
class Compilation;

enum class ActivityKind : uint8_t {
  Input = 0,
  Compile,
  StaticLink,
  DynamicLink,
  ExecLink,
  FirstJobContext = Compile,
  LastJobContext = ExecLink
};

/// A class for parsing a DeclSpecifier.
class Activity {
  ActivityKind kind;
  static const char *GetNameByKind(ActivityKind kind);

public:
  Activity(ActivityKind kind) : kind(kind) {}

public:
  ActivityKind GetKind() const { return kind; }
  const char *GetName() const { return Activity::GetNameByKind(kind); }

  /// Print basic information
  virtual void Print(ColorOutputStream &stream,
                     llvm::StringRef terminator = "\n") const;
};

class InputActity : public Activity {
  const file::File &input;

public:
  InputActity(const file::File &input)
      : Activity(ActivityKind::Input), input(input) {}
  const file::File &GetInput() const { return input; }

public:
  static bool classof(const Activity *unit) {
    return unit->GetKind() == ActivityKind::Input;
  }
};

using ActivityList = llvm::ArrayRef<const Activity *>;
class JobActivity : public Activity {

  const Tool &tool;
  friend class Driver;
  /// Jobs are made via ToolChain::MakeJob(...)
  void *operator new(size_t size) { return ::operator new(size); };

  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<const Activity *> inputs;

public:
  using size_type = llvm::ArrayRef<const Activity *>::size_type;
  using iterator = llvm::ArrayRef<const Activity *>::iterator;
  using const_iterator = llvm::ArrayRef<const Activity *>::const_iterator;

public:
  JobActivity(ActivityKind kind, const Tool &tool, ActivityList inputs,
              file::Type outputFileType)
      : Activity(kind), tool(tool), inputs(inputs),
        outputFileType(outputFileType) {}

public:
  ActivityList GetInputs() const { return inputs; }
  void AddInput(const Activity *input) { inputs.push_back(input); }
  file::Type GetOutputFileType() const { return outputFileType; }
  void SetOutputFileType(file::Type fileType) { outputFileType = fileType; }

  /// Perform a complete dump of this job.
  virtual void Dump(ColorOutputStream &stream,
                    llvm::StringRef terminator = "\n") const;

public:
  size_type size() const { return inputs.size(); }
  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const Activity *unit) {
    return (unit->GetKind() >= ActivityKind::FirstJobContext &&
            unit->GetKind() <= ActivityKind::LastJobContext);
  }
};

class CompileJobActivity final : public JobActivity {
public:
  CompileJobActivity(const Tool &tool, file::Type outputFileType)
      : JobActivity(ActivityKind::Compile, tool, {}, outputFileType) {}

  CompileJobActivity(const Tool &tool, const Activity *input,
                     file::Type outputFileType)
      : JobActivity(ActivityKind::Compile, tool, input, outputFileType) {}

public:
  static bool classof(const Activity *unit) {
    return unit->GetKind() == ActivityKind::Compile;
  }
};
class DynamicLinkJobActivity final : public JobActivity {
  bool requiresLTO;

public:
  DynamicLinkJobActivity(const Tool &tool, ActivityList inputs,
                         file::Type outputFileType, bool requiresLTO = false)
      : JobActivity(ActivityKind::DynamicLink, tool, inputs, outputFileType),
        requiresLTO(requiresLTO) {}

public:
  static bool classof(const Activity *unit) {
    return unit->GetKind() == ActivityKind::DynamicLink;
  }
};

class StaticLinkJobActivity final : public JobActivity {
public:
  StaticLinkJobActivity(const Tool &tool, ActivityList inputs,
                        file::Type outputFileType)
      : JobActivity(ActivityKind::StaticLink, tool, inputs, outputFileType) {}

public:
  static bool classof(const Activity *unit) {
    return unit->GetKind() == ActivityKind::StaticLink;
  }
};

class ExecLinkJobActivity final : public JobActivity {
public:
  ExecLinkJobActivity(const Tool &tool, ActivityList inputs,
                      file::Type outputFileType)
      : JobActivity(ActivityKind::ExecLink, tool, inputs, outputFileType) {}

public:
  static bool classof(const Activity *unit) {
    return unit->GetKind() == ActivityKind::ExecLink;
  }
};

class OutputOptions;
class ActivityCache final {
public:
  /// We keep track of the jobs for the module that we are building.
  /// These are CompileJob
  llvm::SmallVector<const Activity *, 16> forModule;

  /// When are building the Jobs(s), keep track of the linker dependecies
  llvm::SmallVector<const Activity *, 16> forLink;

  /// These are the top-level jobs -- we use them recursively to build
  llvm::SmallVector<const Activity *, 16> forTop;

public:
  bool ForModule() { return forModule.size(); }
  void CacheForModule(const Activity *input) { forModule.push_back(input); }

  bool ForLink() { return forLink.size(); }
  void CacheForLink(const Activity *input) { forLink.push_back(input); }

  bool ForTop() { return forTop.size(); }
  void CacheForTop(const Activity *input) { forTop.push_back(input); }

public:
  void Finish(Compilation &compilation, const OutputOptions &outputOpts);
};

} // namespace stone

#endif
