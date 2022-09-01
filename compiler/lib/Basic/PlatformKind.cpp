
#include "stone/Basic/PlatformKind.h"
#include "stone/Basic/LangOptions.h"
///#include "stone/Basic/Platform.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"

using namespace stone;

StringRef stone::PlatformString(PlatformKind platform) {
  switch (platform) {
  case PlatformKind::none:
    return "*";
#define AVAILABILITY_PLATFORM(X, PrettyName)                                   \
  case PlatformKind::X:                                                        \
    return #X;
#include "stone/Basic/PlatformKind.def"
  }
  llvm_unreachable("bad PlatformKind");
}

StringRef stone::PrettyPlatformString(PlatformKind platform) {
  switch (platform) {
  case PlatformKind::none:
    return "*";
#define AVAILABILITY_PLATFORM(X, PrettyName)                                   \
  case PlatformKind::X:                                                        \
    return PrettyName;
#include "stone/Basic/PlatformKind.def"
  }
  llvm_unreachable("bad PlatformKind");
}

Optional<PlatformKind> stone::PlatformFromString(StringRef Name) {
  if (Name == "*")
    return PlatformKind::none;
  return llvm::StringSwitch<Optional<PlatformKind>>(Name)
#define AVAILABILITY_PLATFORM(X, PrettyName) .Case(#X, PlatformKind::X)
#include "stone/Basic/PlatformKind.def"
      .Case("OSX", PlatformKind::macOS)
      .Default(Optional<PlatformKind>());

  //.Case("OSXApplicationExtension", PlatformKind::macOSApplicationExtension)
}

static bool isPlatformActiveForTarget(PlatformKind Platform,
                                      const llvm::Triple &Target,
                                      bool EnableAppExtensionRestrictions) {
  if (Platform == PlatformKind::none)
    return true;

  // if (Platform == PlatformKind::macOSApplicationExtension ||
  //     Platform == PlatformKind::iOSApplicationExtension ||
  //     Platform == PlatformKind::macCatalystApplicationExtension)
  //   if (!EnableAppExtensionRestrictions)
  //     return false;

  // FIXME: This is an awful way to get the current OS.
  switch (Platform) {
  case PlatformKind::macOS:
    // case PlatformKind::macOSApplicationExtension:
    return Target.isMacOSX();
  case PlatformKind::iOS:
    // case PlatformKind::iOSApplicationExtension:
    return Target.isiOS() && !Target.isTvOS();
  // case PlatformKind::macCatalyst:
  // case PlatformKind::macCatalystApplicationExtension:
  //   return tripleIsMacCatalystEnvironment(Target);
  // case PlatformKind::tvOS:
  // case PlatformKind::tvOSApplicationExtension:
  //   return Target.isTvOS();
  // case PlatformKind::watchOS:
  // case PlatformKind::watchOSApplicationExtension:
  //   return Target.isWatchOS();
  case PlatformKind::OpenBSD:
    return Target.isOSOpenBSD();
  case PlatformKind::Windows:
    return Target.isOSWindows();
  case PlatformKind::none:
    llvm_unreachable("handled above");
  }
  llvm_unreachable("bad PlatformKind");
}

bool stone::IsPlatformActive(PlatformKind Platform, const LangOptions &LangOpts,
                             bool ForTargetVariant) {
  llvm::Triple TT = LangOpts.Target;

  if (ForTargetVariant) {
    assert(LangOpts.TargetVariant && "Must have target variant triple");
    TT = *LangOpts.TargetVariant;
  }

  return isPlatformActiveForTarget(Platform, TT,
                                   LangOpts.EnableAppExtensionRestrictions);
}

PlatformKind stone::TargetPlatform(const LangOptions &LangOpts) {
  if (LangOpts.Target.isMacOSX()) {
    // return (LangOpts.EnableAppExtensionRestrictions
    //             ? PlatformKind::macOSApplicationExtension
    //             : PlatformKind::macOS);
    return PlatformKind::macOS;
  }

  // if (LangOpts.Target.isTvOS()) {
  //   return (LangOpts.EnableAppExtensionRestrictions
  //           ? PlatformKind::tvOSApplicationExtension
  //           : PlatformKind::tvOS);
  // }

  // if (LangOpts.Target.isWatchOS()) {
  //   return (LangOpts.EnableAppExtensionRestrictions
  //           ? PlatformKind::watchOSApplicationExtension
  //           : PlatformKind::watchOS);
  // }

  if (LangOpts.Target.isiOS()) {

    // if (tripleIsMacCatalystEnvironment(LangOpts.Target))
    //   return (LangOpts.EnableAppExtensionRestrictions
    //               ? PlatformKind::macCatalystApplicationExtension
    //               : PlatformKind::macCatalyst);

    // return (LangOpts.EnableAppExtensionRestrictions
    //             ? PlatformKind::iOSApplicationExtension
    //             : PlatformKind::iOS);
    return PlatformKind::iOS;
  }

  return PlatformKind::none;
}

bool stone::InheritsAvailabilityFromPlatform(PlatformKind Child,
                                             PlatformKind Parent) {

  // TODO:
  //  if (Child == PlatformKind::macCatalyst && Parent == PlatformKind::iOS)
  //    return true;

  // if (Child == PlatformKind::macCatalystApplicationExtension) {
  //   if (Parent == PlatformKind::iOS ||
  //       Parent == PlatformKind::iOSApplicationExtension ||
  //       Parent == PlatformKind::macCatalyst) {
  //     return true;
  //   }
  // }
  // Ideally we would have all ApplicationExtension platforms
  // inherit from their non-extension platform.

  return false;
}

llvm::VersionTuple
stone::CanonicalizePlatformVersion(PlatformKind platform,
                                   const llvm::VersionTuple &version) {

  // Canonicalize macOS version for macOS Big Sur to treat
  // 10.16 as 11.0.
  if (platform == PlatformKind::macOS /*||
      platform == PlatformKind::macOSApplicationExtension*/) {
    return llvm::Triple::getCanonicalVersionForOS(llvm::Triple::MacOSX,
                                                  version);
  }
  return version;
}
