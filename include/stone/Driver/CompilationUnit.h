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

enum class CompilationUnitKind : uint8_t {
  Input = 0,
  Compile,
  Link,
  StaticLink,
  DynamicLink,
  ExecLink,
  FirstJob = Compile,
  LastJob = ExecLink,
};

/// A class for parsing a DeclSpecifier.
class CompilationUnit {
  CompilationUnitKind kind;
  static const char *GetNameByKind(CompilationUnitKind kind);

public:
  CompilationUnit(CompilationUnitKind kind) : kind(kind) {}

public:
  CompilationUnitKind GetKind() const { return kind; }
  const char *GetName() const { return CompilationUnit::GetNameByKind(kind); }

  /// Print basic information
  virtual void Print(ColorOutputStream &stream,
                     llvm::StringRef terminator = "\n") const;
};

class CompilationInput : public CompilationUnit {

  const file::File &input;

public:
  CompilationInput(const file::File &input)
      : CompilationUnit(CompilationUnitKind::Input), input(input) {}

  const file::File &GetInput() const { return input; }

public:
  static bool classof(const CompilationUnit *unit) {
    return unit->GetKind() == CompilationUnitKind::Input;
  }
};

// The process ID
using CompilationJobID = int64_t;
enum class CompilationJobStage : uint8_t { None = 0, Running, Finished, Error };
using UnitList = llvm::ArrayRef<const CompilationUnit *>;

class CompilationJob : public CompilationUnit {

  friend ToolChain;
  friend Compilation;

  const Tool &tool;
  CompilationJobID jobID;

  /// Jobs are made via ToolChain::MakeJob(...)
  void *operator new(size_t size) { return ::operator new(size); };

  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<const CompilationUnit *> inputs;

public:
  using size_type = llvm::ArrayRef<const CompilationUnit *>::size_type;
  using iterator = llvm::ArrayRef<const CompilationUnit *>::iterator;
  using const_iterator =
      llvm::ArrayRef<const CompilationUnit *>::const_iterator;

public:
  CompilationJob(CompilationUnitKind kind, const Tool &tool, UnitList inputs,
                 file::Type outputFileType)
      : CompilationUnit(kind), tool(tool), inputs(inputs),
        outputFileType(outputFileType) {}

public:
  UnitList GetInputs() const { return inputs; }
  void AddInput(const CompilationUnit *input) { inputs.push_back(input); }
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

private:
  /// Set when the Compilation adds the job to the JobQueue
  void SetCompilationJobID(CompilationJobID jid) { jobID = jid; }

public:
  // Required for llvm::dyn_cast
  static bool classof(const CompilationUnit *unit) {
    return (unit->GetKind() >= CompilationUnitKind::FirstJob &&
            unit->GetKind() <= CompilationUnitKind::LastJob);
  }
};

class CompileJob final : public CompilationJob {

public:
  CompileJob(const Tool &tool, file::Type outputFileType)
      : CompilationJob(CompilationUnitKind::Compile, tool, {}, outputFileType) {
  }

  CompileJob(const Tool &tool, const CompilationUnit *input,
             file::Type outputFileType)
      : CompilationJob(CompilationUnitKind::Compile, tool, input,
                       outputFileType) {}

public:
  static bool classof(const CompilationUnit *unit) {
    return unit->GetKind() == CompilationUnitKind::Compile;
  }
};

class DynamicLinkJob final : public CompilationJob {
  bool requiresLTO;

public:
  DynamicLinkJob(const Tool &tool, UnitList inputs, file::Type outputFileType,
                 bool requiresLTO = false)
      : CompilationJob(CompilationUnitKind::DynamicLink, tool, inputs,
                       outputFileType),
        requiresLTO(requiresLTO) {}

public:
  static bool classof(const CompilationUnit *unit) {
    return unit->GetKind() == CompilationUnitKind::DynamicLink;
  }
};

class StaticLinkJob final : public CompilationJob {

public:
  StaticLinkJob(const Tool &tool, UnitList inputs, file::Type outputFileType)
      : CompilationJob(CompilationUnitKind::StaticLink, tool, inputs,
                       outputFileType) {}

public:
  static bool classof(const CompilationUnit *unit) {
    return unit->GetKind() == CompilationUnitKind::StaticLink;
  }
};

class ExecLinkJob final : public CompilationJob {
public:
  ExecLinkJob(const Tool &tool, UnitList inputs, file::Type outputFileType)
      : CompilationJob(CompilationUnitKind::ExecLink, tool, inputs,
                       outputFileType) {}

public:
  static bool classof(const CompilationUnit *unit) {
    return unit->GetKind() == CompilationUnitKind::ExecLink;
  }
};

class OutputOptions;
class UnitCache final {
public:
  /// We keep track of the jobs for the module that we are building.
  /// These are CompileJob
  llvm::SmallVector<const CompilationUnit *, 16> forModule;

  /// When are building the Jobs(s), keep track of the linker dependecies
  llvm::SmallVector<const CompilationUnit *, 16> forLink;

  /// These are the top-level jobs -- we use them recursively to build
  llvm::SmallVector<const CompilationUnit *, 16> forTop;

public:
  bool ForModule() { return forModule.size(); }
  void CacheForModule(const CompilationUnit *input) {
    forModule.push_back(input);
  }

  bool ForLink() { return forLink.size(); }
  void CacheForLink(const CompilationUnit *input) { forLink.push_back(input); }

  bool ForTop() { return forTop.size(); }
  void CacheForTop(const CompilationUnit *input) { forTop.push_back(input); }

public:
  void Finish(Compilation &compilation, const OutputOptions &outputOpts);
};

} // namespace stone

#endif
