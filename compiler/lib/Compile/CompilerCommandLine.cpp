#include "stone/Compile/Compiler.h"


CompilerAction::CompilerAction() {}

Status CompilerCommandLine::ParseCompilerAction() { return Status(); }

Status CompilerCommandLine::ParseCompilerOptions() { return Status(); }

Status CompilerCommandLine::ParseCodeGenOptions() { return Status(); }

Status CompilerCommandLine::ParseASTOptions() { return Status(); }

Status CompilerCommandLine::ParseSearchPathOptions() { return Status(); }

Status CompilerCommandLine::ParseTypeCheckerOptions() { return Status(); }

Status CompilerCommandLine::ParseModuleOptions() { return Status(); }

Status
CompilerCommandLine::ParseCommandLine(llvm::ArrayRef<const char *> args) {
  // First, parse the compiler action
 
  return Status();
}