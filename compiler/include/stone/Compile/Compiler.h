#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/CompilerInvocation.h"

#include <deque>

namespace stone {

class Compiler final {


  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

public:
  // This is a simple queue to determine the order of the actions
  std::deque<ActionKind> actions;

  CompilerInvocation invocation;
  CompilerExecution execution;

public:
  Compiler();

  CompilerInvocation &GetInvocation() { return invocation; }
  CompilerExecution &GetExecution() { return execution; }

public:
  void Setup();

public:
  void SetupAction(ActionKind kind);
  void QueueAction(ActionKind kind);
  // Status ForEachAction(std::function<Status(ActionKind kind)>);

public:
  DiagnosticEngine &GetDiags() { return diags; }
  bool HasError() { return diags.HasError(); }
  SrcMgr &GetSrcMgr() { return srcMgr; }

};

} // namespace stone
#endif
