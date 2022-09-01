#include "stone/Basic/LangOptions.h"
#include "llvm/Support/Host.h"

using namespace stone;

LangOptions::LangOptions() : Target(llvm::sys::getDefaultTargetTriple()) {}

std::pair<bool, bool> LangOptions::SetTarget(llvm::StringRef triple) {
  return SetTarget(llvm::Triple(triple));
}

static bool CanSupportOS() { return false; }
static bool CanSupportArch() { return false; }
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
  // Set the "arch" platform condition.
  switch (Target.getArch()) {
  case llvm::Triple::ArchType::arm:
  case llvm::Triple::ArchType::thumb:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "arm");
    break;
  case llvm::Triple::ArchType::aarch64:
  case llvm::Triple::ArchType::aarch64_32:
    if (Target.getArchName() == "arm64_32") {
      AddPlatformConditionValue(PlatformConditionKind::Arch, "arm64_32");
    } else {
      AddPlatformConditionValue(PlatformConditionKind::Arch, "arm64");
    }
    break;
  case llvm::Triple::ArchType::ppc64:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "powerpc64");
    break;
  case llvm::Triple::ArchType::ppc64le:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "powerpc64le");
    break;
  case llvm::Triple::ArchType::x86:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "i386");
    break;
  case llvm::Triple::ArchType::x86_64:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "x86_64");
    break;
  case llvm::Triple::ArchType::systemz:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "s390x");
    break;
  case llvm::Triple::ArchType::wasm32:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "wasm32");
    break;
  default:
    UnsupportedArch = true;
  }

  if (UnsupportedOS || UnsupportedArch)
    return {UnsupportedOS, UnsupportedArch};

  return std::make_pair<bool, bool>(true, true);
}