#include <stdio.h>


compiler.Compile(){
	Compile(GetAction().GetKind());
}

compiler.Compile(ActionKind){
	switch(kind){
	case ActionKind::Parse:
		return CompileForParse();
	case ActionKind::DumpAST:
		return CompileForDumpAST()
	case ActionKind::ResolveImports:
		return CompileForResolveImports()
	case ActionKind::TypeCheck:
		return CompileForTypeCheck()
	case ActionKind::EmitBC:
	case ActionKind::EmitObject:
		return CompileForEmitCode();
	 default:
	 	CompileForAny();
	}
}

void Compiler::CompilerForDumpAST() {
	CompileForAction(ActionKind::Parse);
}
void Compiler::CompilerForResolveImports() {
	CompileForAction(ActionKind::Parse);
}