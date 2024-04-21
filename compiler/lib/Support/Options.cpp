#include "stone/Support/Options.h"
#include "stone/Basic/LLVM.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

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

void opts::PrintArg(ColorStream &outStream, const char *Arg,
                    llvm::StringRef TempDir) {

  const bool Escape = std::strpbrk(Arg, "\"\\$ ");

  if (!TempDir.empty()) {
    llvm::SmallString<256> ArgPath{Arg};
    llvm::sys::fs::make_absolute(ArgPath);
    llvm::sys::path::native(ArgPath);

    llvm::SmallString<256> TempPath{TempDir};
    llvm::sys::fs::make_absolute(TempPath);
    llvm::sys::path::native(TempPath);

    if (StringRef(ArgPath).starts_with(TempPath)) {
      // Don't write temporary file names in the debug info. This would prevent
      // incremental llvm compilation because we would generate different IR on
      // every compiler invocation.
      Arg = "<temporary-file>";
    }
  }
  if (!Escape) {
    outStream << Arg;
    return;
  }
  // Quote and escape. This isn't really complete, but good enough.
  outStream << '"';
  while (const char c = *Arg++) {
    if (c == '"' || c == '\\' || c == '$')
      outStream << '\\';
    outStream << c;
  }
  outStream << '"';
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

// std::unique_ptr<llvm::opt::InputArgList>
// opts::GetInputArgList(llvm::ArrayRef<const char *> args,
//   llvm::opt::OptTable& opts, DiagnosticEngine& diags) {

//   auto ial = std::make_unique<llvm::opt::InputArgList>(
//       opts.ParseArgs(args, missingArgIndex, missingArgCount,
//                           includedFlagsBitmask, excludedFlagsBitmask));
//   assert(ial && "No input argument list.");
//   // Check for missing argument error.
//   if (missingArgCount) {
//     diags.PrintD(
//         SrcLoc(), diag::err_missing_arg_value,
//         diag::LLVMStr(ial->getArgString(missingArgIndex)),
//         diag::UInt(missingArgCount));
//     return nullptr;
//   }
//   // Check for unknown arguments.
//   for (const llvm::opt::Arg *arg : ial->filtered(opts::UNKNOWN)) {
//     GetLangContext().GetDiags().PrintD(SrcLoc(), diag::err_unknown_arg,
//                                        diag::LLVMStr(arg->getAsString(*ial)));
//     return nullptr;
//   }
//   return ial;
// }

namespace {
class StoneOptTable : public llvm::opt::GenericOptTable {
public:
  StoneOptTable() : GenericOptTable(InfoTable) {}
};
} // end anonymous namespace

Options::Options()
    : optTable(std::unique_ptr<GenericOptTable>(new StoneOptTable())) {}

/// Print the options
void Options::PrintHelp(ColorStream &out) const {}
