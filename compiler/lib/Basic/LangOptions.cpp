#include "stone/Basic/LangOptions.h"
#include "llvm/Support/Host.h"

using namespace stone;

LangOptions::LangOptions() : Target(llvm::sys::getDefaultTargetTriple()) {}

std::pair<bool, bool> LangOptions::SetTarget(llvm::StringRef triple) {
  return SetTarget(llvm::Triple(triple));
}
std::pair<bool, bool> LangOptions::SetTarget(llvm::Triple triple) {

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
  Target = std::move(triple);

  bool UnsupportedOS = false;
  // Set the "os" platform condition.
  switch (Target.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX:
    AddPlatformConditionValue(PlatformConditionKind::OS, "OSX");
    break;
  case llvm::Triple::IOS:
    AddPlatformConditionValue(PlatformConditionKind::OS, "iOS");
    break;
  case llvm::Triple::Linux:
    if (Target.getEnvironment() == llvm::Triple::Android) {
      AddPlatformConditionValue(PlatformConditionKind::OS, "Android");
    } else {
      AddPlatformConditionValue(PlatformConditionKind::OS, "Linux");
    }
    break;
  case llvm::Triple::FreeBSD:
    AddPlatformConditionValue(PlatformConditionKind::OS, "FreeBSD");
    break;
  case llvm::Triple::OpenBSD:
    AddPlatformConditionValue(PlatformConditionKind::OS, "OpenBSD");
    break;
  default:
    UnsupportedOS = true;
    break;
  }
  bool UnsupportedArch = false;

  return std::make_pair<bool, bool>(true, true);
}