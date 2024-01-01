#ifndef STONE_DRIVER_PRETTYSTACKTRACE_H
#define STONE_DRIVER_PRETTYSTACKTRACE_H

#include "llvm/Support/PrettyStackTrace.h"

namespace stone {

class CompilationEntity;

class CompilationEntityPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  const CompilationEntity *entity;
  const char *description;

public:
  CompilationEntityPrettyStackTrace(const char *description,
                                    const CompilationEntity *entity)
      : entity(entity), description(description) {}

public:
  void print(llvm::raw_ostream &OS) const override;
};
} // namespace stone

#endif