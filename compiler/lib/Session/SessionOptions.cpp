#include "stone/Session/SessionOptions.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"

using namespace stone;

SessionOptions::SessionOptions(std::unique_ptr<Mode> m) : mode(std::move(m)) {
  llvm::sys::fs::current_path(currentPath);
}

// TODO:
//  SessionOptions::SessionOptions() :
//  target(llvm::sys::getDefaultTargetTriple()) {
//  }

// void SessionOptions::SetTargetTriple(llvm::StringRef triple) {
//   SetTargetTriple(llvm::Triple(triple));
// }

// void SessionOptions::SetTargetTriple(const llvm::Triple &triple) {
//   /// TODO: Messy -- think of a cleaner way
//   // if (triple.getOS() == llvm::Triple::Darwin &&
//   //     triple.getVendor() == llvm::Triple::Apple) {
//   //   // Rewrite darwinX.Y triples to macosx10.X'.Y ones.
//   //   // It affects code generation on our platform.
//   //   llvm::SmallString<16> osxBuf;
//   //   llvm::raw_svector_ostream osx(osxBuf);
//   //   osx << llvm::Triple::getOSTypeName(llvm::Triple::MacOSX);

//   //   unsigned major, minor, micro;
//   //   triple.getMacOSXVersion(major, minor, micro);
//   //   osx << major << "." << minor;
//   //   if (micro != 0) {
//   //     osx << "." << micro;
//   //   }
//   //   triple.setOSName(osx.str());
//   // }
//   target = std::move(triple);
// }