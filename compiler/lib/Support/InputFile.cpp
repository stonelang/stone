#include "stone/Support/InputFile.h"

#include "llvm/ADT/StringRef.h"

llvm::StringRef
stone::ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(
    llvm::StringRef filename) {
  if (filename.equals("<stdin>")) {
    return "-";
  }
  return filename;
}