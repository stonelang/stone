#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Compile/Compile.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"

using namespace stone;

// class InTextParser {

//   const char *CurPtr;
//   const char *TailPtr;

//   enum Kind { Percent, Number, Letter, LBrace, RBrace };

// public:
//   InTextParser(const char *CurPtr, const char *TailPtr)
//       : CurPtr(CurPtr), TailPtr(TailPtr) {}

//   InTextParser(llvm::StringRef Text)
//       : DiagosticTextParser(Text.begin(), Text.end()) {}

// public:
//   void Parse(llvm::SmallVector<char> &results) {
//     while (CurPtr != TailPtr) {
//       switch (Parse(CurPtr)) {
//       case DiagosticTextParser::Kind::Percent: {
//         break;
//       }
//       case DiagosticTextParser::Kind::Number: {
//         break;
//       }
//       case DiagosticTextParser::Kind::Letter: {
//         break;
//       }
//       }
//     }
//   }
//   DiagosticTextParser::Kind Parse(const char *CurPtr) {
//     switch (*CurPtr++) {
//     case '%': {
//       return DiagosticTextParser::Percent;
//     }
//     case '{': {
//       return DiagosticTextParser::LBrace;
//     }
//     case '}': {
//       return DiagosticTextParser::RBrace;
//     }
//     case '0': {
//     case '1':
//     case '2':
//     case '3':
//     case '4':
//     case '5':
//     case '6':
//     case '7':
//     case '8':
//     case '9':
//       return DiagosticTextParser::Number;
//     }
//     default: {
//       return DiagosticTextParser::Letter;
//     }
//     }
//   }
// };

int main(int argc, const char **args) {
  START_LLVM_INIT(argc, args);
  llvm::SmallVector<char> results;

  llvm::SmallVector<const char *, 256> argsToExpand(args, args + argc);
  llvm::BumpPtrAllocator ptrAlloc;
  llvm::StringSaver strSaver(ptrAlloc);
  llvm::cl::ExpandResponseFiles(
      strSaver,
      llvm::Triple(llvm::sys::getProcessTriple()).isOSWindows()
          ? llvm::cl::TokenizeWindowsCommandLine
          : llvm::cl::TokenizeGNUCommandLine,
      argsToExpand);

  llvm::ArrayRef<const char *> argv(argsToExpand);
  return stone::Compile(
      llvm::ArrayRef(argv.data() + 1, argv.data() + argv.size()), argv[0],
      (void *)(intptr_t)stone::GetMainExecutablePath, nullptr);
}