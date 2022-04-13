#ifndef STONE_GEN_IRCODEGENRESULT_H
#define STONE_GEN_IRCODEGENRESULT_H

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRCodeGenResult final {
  std::unique_ptr<llvm::Module> llvmModule;

public:
  /// Return the module that this code generator is building into.
  ///
  /// This may return null after HandleTranslationUnit is called;
  /// this signifies that there was an error generating code.  A
  /// diagnostic will have been generated in this case, and the module
  /// will be deleted.
  ///
  /// It will also return null if the module is released.
  llvm::Module *GetLLVMModule();
  llvm::Module *ReleaseLLVMModule();
};
} // namespace stone

#endif
