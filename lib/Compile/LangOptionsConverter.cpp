#include "stone/Compile/LangOptionsConverter.h"
#include "stone/Basic/CompileDiagnostic.h"
//#include "stone/Basic/Platform.h"
#include "stone/Compile/LangInputsConverter.h"
#include "stone/Compile/LangOutputsConverter.h"
#include "stone/Session/Options.h"
//#include "stone/Session/SanitizerOptions.h"
#include "stone/Parse/Lexer.h"
//#include "stone/Strings.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

stone::Error LangOptionsConverter::Convert(
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  // TODO: OK for now
  assert(langOpts.inputsAndOutputs.HasInputs() &&
         "Inputs and Outputs should be empty");

  // llvm::Optional<LangInputsAndOutputs> inputsAndOutputs =
  //     LangInputsConverter(de, args).Convert(buffers);

  return stone::Error();
}
