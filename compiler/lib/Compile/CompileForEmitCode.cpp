#include "stone/Compile/CompilerInstance.h"

using namespace stone;

Status CompilerInstance::CompileForEmitCode() {
	Compile(ModeKind::TypeCheck);

	return Status();
}