#include "stone/Options/Options.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;

using namespace llvm::opt;

#define PREFIX(NAME, VALUE) static const char *const NAME[] = VALUE;
#include "stone/Options/StoneOpts.inc"
#undef PREFIX

static const OptTable::Info InfoTable[] = {
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM,  \
               HELPTEXT, METAVAR, VALUES)                                      \
  {PREFIX, NAME,  HELPTEXT, METAVAR, ID,        Option::KIND##Class,           \
   PARAM,  FLAGS, GROUP,    ALIAS,   ALIASARGS, VALUES},
#include "stone/Options/StoneOpts.inc"
#undef OPTION
};

namespace stone {
class StoneOptTable : public llvm::opt::OptTable {
public:
  StoneOptTable() : OptTable(InfoTable) {}
};
} // namespace stone

std::unique_ptr<llvm::opt::OptTable> stone::opts::CreateOptTable() {
  return std::unique_ptr<llvm::opt::OptTable>(new stone::StoneOptTable());
}

OptTableAndInputArgs stone::opts::ParseArgs(llvm::ArrayRef<const char *> args,
                                            OptParsingFlags flags) {
  auto opts = stone::opts::CreateOptTable();
  auto ial = std::make_unique<llvm::opt::InputArgList>(
      opts->ParseArgs(args, flags.missingArgIndex, flags.missingArgCount,
                      flags.includedFlagsBitmask, flags.excludedFlagsBitmask));

  assert(ial && "No input argument list.");
  // Check for missing argument error.
  // if (flags.missingArgCount) {
  //   GetLang().GetDiags().PrintD(
  //       SrcLoc(), diag::err_missing_arg_value,
  //       diag::LLVMStr(ial->getArgString(missingArgIndex)),
  //       diag::UInt(missingArgCount));
  //   return nullptr;
  // }
  // // Check for unknown arguments.
  // for (const llvm::opt::Arg *arg : ial->filtered(opts::UNKNOWN)) {
  //   GetLang().GetDiags().PrintD(
  //       SrcLoc(), diag::err_unknown_arg,
  //       diag::LLVMStr(arg->getAsString(*ial)));
  //   return nullptr;
  // }
  return std::make_pair(std::move(opts), std::move(ial));
}

// stone::Result<std::string>
// stone::opts::GetOptEqualValue(opts::OptID optID,
//                           const llvm::opt::InputArgList &ial) {
//   if (ial.hasArg(optID)) {
//     auto arg = ial.getLastArg(optID);
//     if (arg) {
//       return stone::Result<std::string>(arg->getValue());
//     }
//   }
//   return stone::Result<std::string>();
// }
