#ifndef STONE_DRIVER_PRETTYSTACKTRACE_H
#define STONE_DRIVER_PRETTYSTACKTRACE_H

#include "stone/Basic/File.h"
#include "llvm/Support/PrettyStackTrace.h"

namespace stone {

class Job;
class JobConstruction;
class CommandOutput;

class JobConstructionPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const JobConstruction *construction;
  const char *description;

public:
  JobConstructionPrettyStackTrace(const char *description,
                                  const JobConstruction *construction)
      : construction(construction), description(description) {}

public:
  void print(llvm::raw_ostream &OS) const override;
};

class JobPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const Job *job;
  const char *description;

public:
  JobPrettyStackTrace(const char *description, const Job *job)
      : job(job), description(description) {}
  void print(llvm::raw_ostream &OS) const override;
};

} // namespace stone

#endif