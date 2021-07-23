#ifndef STONE_CODEGEN_CODEGENERATOR_H
#define STONE_CODEGEN_CODEGENERATOR_H

#include "stone/Basic/LLVM.h"
#include "stone/Syntax/Module.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
namespace codegen {
class CodeGenModule;
class CodeGenerator final {
public:
  CodeGenerator();

public:
  /// Return an opaque reference to the CodeGenModule object, which can
  /// be used in various secondary APIs.  It is valid as long as the
  /// Generator exists.
  CodeGenModule &GetCodeGenModule();

  /// Return the module that this code generator is building into.
  ///
  /// This may return null after HandleTranslationUnit is called;
  /// this signifies that there was an error generating code.  A
  /// diagnostic will have been generated in this case, and the module
  /// will be deleted.
  ///
  /// It will also return null if the module is released.
  llvm::Module *GetModule();
};
} // namespace codegen
} // namespace stone
#endif
