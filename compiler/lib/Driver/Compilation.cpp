#include "stone/Driver/Compilation.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"

using namespace stone;

Compilation::Compilation(const Driver &driver) : driver(driver) {}

CompilationResult::CompilationResult() {}

class CompilationJobScheduler {
  Compilation &compilation;

public:
  CompilationJobScheduler(Compilation &compilation)
      : compilation(compilation) {}
  ~CompilationJobScheduler() = default;

public:
  CompilationResult Run();
  Status Finish();
};

CompilationResult CompilationJobScheduler::Run() { return CompilationResult(); }
Status CompilationJobScheduler::Finish() { return Status(); }

CompilationResult Compilation::ExecuteJobs() {
  CompilationJobScheduler compilationJobScheduler(*this);
  STONE_DEFER { compilationJobScheduler.Finish(); };
  return compilationJobScheduler.Run();
}

/// Print the list of Actions in a Compilation.
void Compilation::PrintJobs(llvm::raw_ostream &os) const {}
