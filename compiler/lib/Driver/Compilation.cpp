#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

Compilation::Compilation(const Driver &driver) : driver(driver) {}

void Compilation::AddTopLevelJob(const Job *job) {
  assert(job);
  assert(job->HasTopLevel());
  topLevelJobs.push_back(job);
}

CompilationResult::CompilationResult() {}

class ExecuteJobsImpl {
  Compilation &compilation;

public:
  ExecuteJobsImpl(Compilation &compilation) : compilation(compilation) {}
  ~ExecuteJobsImpl() = default;

public:
};

CompilationResult Compilation::ExecuteJobs() { return CompilationResult(); }

/// Print the list of Actions in a Compilation.
void Compilation::PrintJobs(llvm::raw_ostream &os) const {}
