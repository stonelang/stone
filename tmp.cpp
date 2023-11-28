#include <stdio.h>


class StatisticTracer {

}
class CompilerStatisticTracer : StatisticTracer {
public:
	~CompilerStatisticTracer() {
		engine.SaveTracer(*this);
	}
}
class StatisticEngine {
	SmallVector<Statistic> stats;


public:
	void SaveTracer(StatisticTracer* tracer) {
		stats.push_back(new Statistic(....))
	}

}






public struct IRCodeGenModuleFlusher {

  private IRCodeGenModule &irCodeGenModule;
  private llvm::PassBuilder pb;
  private llvm::LoopAnalysisManager lam;
  private llvm::FunctionAnalysisManager fam;
  private llvm::CGSCCAnalysisManager cgam;
  private llvm::ModuleAnalysisManager mam;
  private llvm::ModulePassManager mpm;
  private llvm::legacy::PassManager lpm;
  private llvm::legacy::FunctionPassManager lfpm;
  private llvm::FunctionPassManager fpm;

  public IRCodeGenModuleFlusher(IRCodeGenModule &irCodeGenModule) {
  	this.irCodeGenModule = irCodeGenModule;
  }
  public ~IRCodeGenModuleFlusher(){}
  
  public fun Flush() -> void  {}
  public fun GetPassBuilder() -> PassBuilder &{ return pb; }

  public fun GetLoopAnalysisManager() -> LoopAnalysisManager & { return lam; }

  public fun GetFunctionAnalysisManager() -> FunctionAnalysisManager &{ return fam; }

  public fun GetCGSCCAnalysisManager() -> CGSCCAnalysisManager &{ return cgam; }

  public fun GetModuleAnalysisManager() -> ModuleAnalysisManager& { return mam; }

}


public struct IRCodeGenModuleFlusher {

  private IRCodeGenModule &irCodeGenModule;
  private llvm::PassBuilder pb;
  private llvm::LoopAnalysisManager lam;
  private llvm::FunctionAnalysisManager fam;
  private llvm::CGSCCAnalysisManager cgam;
  private llvm::ModuleAnalysisManager mam;
  private llvm::ModulePassManager mpm;
  private llvm::legacy::PassManager lpm;
  private llvm::legacy::FunctionPassManager lfpm;
  private llvm::FunctionPassManager fpm;

}

public IRCodeGenModuleFlusher::IRCodeGenModuleFlusher(IRCodeGenModule &irCodeGenModule) {
  	this.irCodeGenModule = irCodeGenModule;
}
public fun IRCodeGenModuleFlusher::Flush() -> void {

}