#include "stone/Basic/LangOptions.h"
#include "llvm/TargetParser/Host.h"

using namespace stone;

LangOptions::LangOptions()
    : DefaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {}

LangOptions::TargetResult LangOptions::SetTarget(llvm::StringRef triple) {
  return SetTarget(llvm::Triple(triple));
}

struct SupportedConditionalValue final {
  llvm::StringRef value;

  /// If the value has been deprecated, the new value to replace it with.
  llvm::StringRef replacement = "";

  SupportedConditionalValue(const char *value) : value(value) {}
  SupportedConditionalValue(const char *value, const char *replacement)
      : value(value), replacement(replacement) {}
};

static const SupportedConditionalValue SupportedConditionalCompilationOSs[] = {
    "OSX",     "macOS",   "tvOS",    "watchOS", "iOS",    "Linux", "FreeBSD",
    "OpenBSD", "Windows", "Android", "PS4",     "Cygwin", "Haiku", "WASI",
};

static const SupportedConditionalValue SupportedConditionalCompilationArches[] =
    {
        "arm",       "arm64",       "arm64_32", "i386",   "x86_64",  "powerpc",
        "powerpc64", "powerpc64le", "s390x",    "wasm32", "riscv64",
};

static const SupportedConditionalValue
    SupportedConditionalCompilationEndianness[] = {"little", "big"};

static bool CanSupportOS() { return false; }
static bool CanSupportArch() { return false; }

LangOptions::TargetResult LangOptions::SetTarget(llvm::Triple triple) {

  LangOptions::TargetResult result;
  if (triple.getOS() == llvm::Triple::Darwin &&
      triple.getVendor() == llvm::Triple::Apple) {
    // Rewrite darwinX.Y triples to macosx10.X'.Y ones.
    // It affects code generation on our platform.
    llvm::SmallString<16> osxBuf;
    llvm::raw_svector_ostream osx(osxBuf);
    osx << llvm::Triple::getOSTypeName(llvm::Triple::MacOSX);

    llvm::VersionTuple OSVersion;
    triple.getMacOSXVersion(OSVersion);

    osx << OSVersion.getMajor() << "." << OSVersion.getMinor().value_or(0);
    if (auto Subminor = OSVersion.getSubminor())
      osx << "." << *Subminor;

    triple.setOSName(osx.str());
  }
  DefaultTargetTriple = std::move(triple);
  // Set the "os" platform condition.
  switch (DefaultTargetTriple.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX:
    AddPlatformConditionValue(PlatformConditionKind::OS, "OSX");
    break;
  case llvm::Triple::IOS:
    AddPlatformConditionValue(PlatformConditionKind::OS, "iOS");
    break;
  case llvm::Triple::Linux:
    if (DefaultTargetTriple.getEnvironment() == llvm::Triple::Android) {
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
    result.UnsupportedOS = true;
    break;
  }

  // Set the "arch" platform condition.
  switch (DefaultTargetTriple.getArch()) {
  case llvm::Triple::ArchType::arm:
  case llvm::Triple::ArchType::thumb:
    AddPlatformConditionValue(PlatformConditionKind::Arch, "arm");
    break;
  case llvm::Triple::ArchType::aarch64:
  case llvm::Triple::ArchType::aarch64_32:
    if (DefaultTargetTriple.getArchName() == "arm64_32") {
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
    result.UnsupportedArch = true;
  }
  if (result.IsUnsupported()) {
    return result;
  }
  // Set the "_endian" platform condition.
  switch (DefaultTargetTriple.getArch()) {
  default:
    llvm_unreachable("undefined architecture endianness");
  case llvm::Triple::ArchType::arm:
  case llvm::Triple::ArchType::thumb:
  case llvm::Triple::ArchType::aarch64:
  case llvm::Triple::ArchType::aarch64_32:
  case llvm::Triple::ArchType::ppc64le:
  case llvm::Triple::ArchType::wasm32:
  case llvm::Triple::ArchType::x86:
  case llvm::Triple::ArchType::x86_64:
    AddPlatformConditionValue(PlatformConditionKind::Endianness, "little");
    break;
  case llvm::Triple::ArchType::ppc64:
  case llvm::Triple::ArchType::systemz:
    AddPlatformConditionValue(PlatformConditionKind::Endianness, "big");
    break;
  }

  // Set the pointer authentication scheme.
  if (DefaultTargetTriple.getArchName() == "arm64e") {
    AddPlatformConditionValue(PlatformConditionKind::PtrAuth, "_arm64e");
  } else {
    AddPlatformConditionValue(PlatformConditionKind::PtrAuth, "_none");
  }

  return result;
}