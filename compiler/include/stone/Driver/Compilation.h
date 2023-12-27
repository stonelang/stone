#ifndef STONE_DRIVER_COMPILATION_H
#define STONE_DRIVER_COMPILATION_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/TaskQueue.h"
#include "stone/Driver/ToolChain.h"

namespace stone {

class Job;
class TaskQueue;
class BuildSystem;
class Compilation;
class CompilationObserver;

class CompilationStats final : public Stats {
  Compilation &compilation;

public:
  CompilationStats(Compilation &compilation)
      : Stats("Compilation statistics:"), compilation(compilation) {}
  void Print(ColorStream &stream) override;
};

/// This mode controls the compilation process
/// p := -primary-file
enum class CompilationKind : uint8_t {
  /// n inputs, n compile(s), n * n  parses
  /// Ex: compile_1(1=p ,...,n), compile_2(1,2=p,...,n),...,
  /// compile_n(1,....,n=p)
  Quadratic = 0,
  /// n input(s), n compile(s), n parses
  /// Ex: compile_1(1=p), compile_2(2=p),..., compile_n(n=p)
  Flat,
  /// n inputs, j CPU(s), j compile(s), n * j parses
  /// Ex: compile_1(1=p,...,n),...,
  /// compile_2(1,2=p,...,n),...,compile_j(1,...,p=j,...,n)
  NCPU,
  /// n inputs, 1 compile, n parses
  /// Ex: compile(1,....,n)
  Single,
};

// class Compilation final {
//   friend CompilationStats;

//   Driver &driver;
//   ToolChain &tc;

//   std::unique_ptr<CompilationStats> stats;
//   std::unique_ptr<TaskQueue> tq;
//   std::unique_ptr<llvm::opt::DerivedArgList> dal;

//   // All the jobs the tool chain created --- lifetime management.
//   llvm::SmallVector<std::unique_ptr<const Job>, 32> jobs;

//   CompilationObserver *observer;

// public:
//   Compilation(Driver &driver, ToolChain &tc,
//               std::unique_ptr<llvm::opt::DerivedArgList> dal);

// public:
//   // template <typename T, typename... Args> T *CreateJob(Args &&...args) {
//   //   auto result = new T(std::forward<Args>(args)...);
//   //   jobs.emplace_back(result);
//   //   return result;
//   // }

// public:
//   // void CancelJob();
//   // void CancelJobs();
//   // void PruneJob();

//   void PrintJobs();
//   stone::Error RunJobs();

//   TaskQueue &GetQueue() { return *tq.get(); }

// public:
//   Driver &GetDriver() { return driver; }
//   ToolChain &GetToolChain() { return tc; }

// public:
//   bool HasObserver() { return observer != nullptr; }
//   void SetObserver(CompilerObserver *inputObserver) {
//     observer = inputObserver;
//   }
//   CompilationObserver *GetObserver() { return observer; }
// };

using CompilationJobList =
    llvm::SmallVector<std::unique_ptr<const CompilationJob>, 32>;

class Compilation {

protected:
  Driver &driver;
  CompilationKind kind;

  CompilationJobList jobs;

public:
  Compilation(CompilationKind kind, Driver &driver);
  ~Compilation();

public:
  CompilationKind GetKind() { return kind; }
  Driver &GetDriver() { return driver; }

public:
};

class QuadraticCompilation final : public Compilation {

public:
  QuadraticCompilation(Driver &driver)
      : Compilation(CompilationKind::Quadratic, driver) {}
};

class FlatCompilation final : public Compilation {

public:
  FlatCompilation(Driver &driver)
      : Compilation(CompilationKind::Flat, driver) {}
};

class NCPUCompilation final : public Compilation {

public:
  NCPUCompilation(Driver &driver)
      : Compilation(CompilationKind::NCPU, driver) {}
};

class SingleCompilation final : public Compilation {

public:
  SingleCompilation(Driver &driver)
      : Compilation(CompilationKind::Single, driver) {}
};

} // namespace stone

#endif
