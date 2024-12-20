#include "stone/Support/Options.h"
#include "stone/Basic/LLVM.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"

using namespace stone;
using namespace stone::opts;

using namespace llvm::opt;

#define OPTTABLE_VALUES_CODE
#include "stone/Support/Options.inc"
#undef OPTTABLE_VALUES_CODE

#define PREFIX(NAME, VALUE)                                                    \
  static constexpr llvm::StringLiteral NAME##_init[] = VALUE;                  \
  static constexpr llvm::ArrayRef<llvm::StringLiteral> NAME(                   \
      NAME##_init, std::size(NAME##_init) - 1);
#include "stone/Support/Options.inc"
#undef PREFIX

static constexpr const llvm::StringLiteral PrefixTable_init[] =
#define PREFIX_UNION(VALUES) VALUES
#include "stone/Support/Options.inc"
#undef PREFIX_UNION
    ;
static constexpr const llvm::ArrayRef<llvm::StringLiteral>
    PrefixTable(PrefixTable_init, std::size(PrefixTable_init) - 1);

static constexpr llvm::opt::GenericOptTable::Info InfoTable[] = {
#define OPTION(...) LLVM_CONSTRUCT_OPT_INFO(__VA_ARGS__),
#include "stone/Support/Options.inc"
#undef OPTION
};

namespace {
class StoneOptTable : public llvm::opt::GenericOptTable {
public:
  StoneOptTable() : GenericOptTable(InfoTable) {}
};
} // end anonymous namespace

Options::Options()
    : optTable(
          std::unique_ptr<llvm::opt::GenericOptTable>(new StoneOptTable())),
      defaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {

  llvm::sys::fs::current_path(workingDirectory);
}

bool Options::IsUnknownOS() const {
  return currentTriple.getOS() == llvm::Triple::UnknownOS;
}

bool Options::IsDarwinOS() const {
  return (currentTriple.getOS() == llvm::Triple::Darwin);
}
bool Options::IsMacOSX() const {
  return (currentTriple.getOS() == llvm::Triple::MacOSX);
}

bool Options::IsLinuxOS() const {
  return (currentTriple.getOS() == llvm::Triple::Linux &&
          !currentTriple.isAndroid());
}
bool Options::IsAndroidOS() const {
  return (currentTriple.getOS() == llvm::Triple::Linux &&
          currentTriple.isAndroid());
}
bool Options::IsFreeBSDOS() const {
  return (currentTriple.getOS() == llvm::Triple::FreeBSD);
}
bool Options::IsOpenBSDOS() const {
  return (currentTriple.getOS() == llvm::Triple::OpenBSD);
}

bool Options::IsWin32OS() const {
  return (currentTriple.getOS() == llvm::Triple::Win32);
}

bool Options::HasCurrentTriple(llvm::Triple triple) {
  switch (triple.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX:
  case llvm::Triple::Linux:
  case llvm::Triple::FreeBSD:
  case llvm::Triple::OpenBSD:
  case llvm::Triple::Win32:
  case llvm::Triple::Haiku:
    return true;
  case llvm::Triple::UnknownOS:
    return false;
  }
  llvm_unreachable("Unknown Triple");
}

/// Print the options
void Options::PrintHelp(bool showHidden) {

  includedFlagsBitmask = 0;
  if (!showHidden) {
    excludedFlagsBitmask |= llvm::opt::HelpHidden;
  }
  GetOptTable().printHelp(
      llvm::outs(), GetMainExecutableName().data(),
      "Stone is a compiler tool for compiling Stone source code.",
      includedFlagsBitmask,
      excludedFlagsBitmask /* must be set -- specific to compiler and driver*/,
      /*ShowAllAliases*/ false);
}

llvm::StringRef
opts::GetEqualValueByOptID(const opts::OptID optID,
                           const llvm::opt::InputArgList &args) {
  if (args.hasArg(optID)) {
    auto arg = args.getLastArg(optID);
    if (arg) {
      return llvm::StringRef(arg->getValue());
    }
  }
  return llvm::StringRef();
}

unsigned opts::GetArgID(const llvm::opt::Arg *arg) {
  assert(arg);
  return arg->getOption().getID();
}
llvm::StringRef opts::GetArgName(const llvm::opt::Arg *arg) {
  assert(arg);
  return arg->getOption().getName();
}
