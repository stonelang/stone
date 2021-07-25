#include "stone/Basic/Basic.h"
#include "stone/Basic/CoreDiagnostic.h"

#include "llvm/Support/Host.h"

using namespace stone;

Basic::Basic()
    : fm(fsOpts), de(diagOpts), cos(llvm::outs()),
      targetTriple(llvm::sys::getDefaultTargetTriple()) {}

Basic::~Basic() {}

unsigned Basic::MakeSrcID(llvm::StringRef filePath) {

  auto fileBuffer = fm.getBufferForFile(filePath);
  if (!fileBuffer) {
    Diagnose(SrcLoc(), diag::err_unable_to_open_filebuffer,
             diag::LLVMStrArgument(filePath));
    return 0;
  }
  return sm.addNewSourceBuffer(std::move(*fileBuffer));
}

void Basic::Panic() { assert(false && "Compiler cannot continue"); }
