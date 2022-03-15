#include "stone/Core/Context.h"
#include "stone/Core/CoreDiagnostic.h"

#include "llvm/Support/Host.h"

using namespace stone;

Target::Target() : triple(llvm::sys::getDefaultTargetTriple()) {}

void Target::Init(const llvm::Triple &triple) {

  void Target::Init(llvm::StringRef triple) {}

  Context::Context()
      : fm(GetFileSystemOptions()), de(GetDiagOptions()), cos(llvm::outs()),
        targetTriple(llvm::sys::getDefaultTargetTriple()) {}

  Context::~Context() {}

  void Context::SetTargetTriple(llvm::StringRef triple) {
    SetTargetTriple(llvm::Triple(triple));
  }

  void Context::SetTargetTriple(const llvm::Triple &triple) {

    GetTarget().Init(triple);

    if (triple.getOS() == llvm::Triple::Darwin &&
        triple.getVendor() == llvm::Triple::Apple) {
      // Rewrite darwinX.Y triples to macosx10.X'.Y ones.
      // It affects code generation on our platform.
      llvm::SmallString<16> osxBuf;
      llvm::raw_svector_ostream osx(osxBuf);
      osx << llvm::Triple::getOSTypeName(llvm::Triple::MacOSX);

      unsigned major, minor, micro;
      triple.getMacOSXVersion(major, minor, micro);
      osx << major << "." << minor;
      if (micro != 0) {
        osx << "." << micro;
      }
      triple.setOSName(osx.str());
    }
    GetSystemOptions().target = std::move(triple);
  }

  // TODO: Remove
  void stone::Panic() { assert(false && "Compiler cannot continue!"); }
  void stone::Panic(const char *msg) { llvm_unreachable(msg); }
