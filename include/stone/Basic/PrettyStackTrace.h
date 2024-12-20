#ifndef STONE_BASIC_PRETTYSTACKTRACE_H
#define STONE_BASIC_PRETTYSTACKTRACE_H

#include "stone/Basic/SrcLoc.h"
#include "llvm/Support/PrettyStackTrace.h"

namespace stone {
/// If a crash happens while one of these objects are live, the message
/// is printed out along with the specified source location.
class PrettyStackTraceLoc final : public llvm::PrettyStackTraceEntry {
  SrcMgr &sm;
  SrcLoc sl;
  const char *message;

public:
  PrettyStackTraceLoc(SrcMgr &sm, SrcLoc sl, const char *msg)
      : sm(sm), sl(sl), message(msg) {}
  // TODO:
  void print(raw_ostream &OS) const override;
};
} // namespace stone
#endif
