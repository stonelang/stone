#include "stone/Option/Options.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::opts;

using namespace llvm::opt;

#define PREFIX(NAME, VALUE) static const char *const NAME[] = VALUE;
#include "stone/Option/Options.inc"
#undef PREFIX

static const OptTable::Info InfoTable[] = {
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM,  \
               HELPTEXT, METAVAR, VALUES)                                      \
  {PREFIX, NAME,  HELPTEXT, METAVAR, ID,        Option::KIND##Class,           \
   PARAM,  FLAGS, GROUP,    ALIAS,   ALIASARGS, VALUES},
#include "stone/Option/Options.inc"
#undef OPTION
};

namespace stone {
class StoneOptTable : public llvm::opt::OptTable {
public:
  StoneOptTable() : OptTable(InfoTable) {}
};
} // namespace stone

Options::Options() : options(opts::CreateOptTable()) {}

std::unique_ptr<OptTable> opts::CreateOptTable() {
  return std::unique_ptr<OptTable>(new stone::StoneOptTable());
}

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

    if (StringRef(ArgPath).startswith(TempPath)) {
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
opts::GetEqualValueByOptionID(const opts::OptID optID,
                              const llvm::opt::InputArgList &args) {
  if (args.hasArg(optID)) {
    auto arg = args.getLastArg(optID);
    if (arg) {
      return llvm::StringRef(arg->getValue());
    }
  }
  return llvm::StringRef();
}

const unsigned opts::GetArgID(llvm::opt::Arg *arg) {
  assert(arg);
  return arg->getOption().getID();
}
llvm::StringRef opts::GetArgName(llvm::opt::Arg *arg) {
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
