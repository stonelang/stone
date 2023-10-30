#include "stone/Session/Options.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;

using namespace llvm::opt;

#define PREFIX(NAME, VALUE) static const char *const NAME[] = VALUE;
#include "stone/Session/StoneOptions.inc"
#undef PREFIX

static const OptTable::Info InfoTable[] = {
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM,  \
               HELPTEXT, METAVAR, VALUES)                                      \
  {PREFIX, NAME,  HELPTEXT, METAVAR, ID,        Option::KIND##Class,           \
   PARAM,  FLAGS, GROUP,    ALIAS,   ALIASARGS, VALUES},
#include "stone/Session/StoneOptions.inc"
#undef OPTION
};

namespace stone {
class StoneOptTable : public llvm::opt::OptTable {
public:
  StoneOptTable() : OptTable(InfoTable) {}
};
} // namespace stone

std::unique_ptr<OptTable> stone::opts::CreateOptTable() {
  return std::unique_ptr<OptTable>(new stone::StoneOptTable());
}

std::unique_ptr<llvm::opt::InputArgList> 
stone::opts::CreateInputArgList(llvm::ArrayRef<const char *> args, unsigned includedFlagsBitmask, 
  unsigned excludedFlagsBitmask, unsigned missingArgIndex, unsigned missingArgCount){

  auto ial = std::make_unique<llvm::opt::InputArgList>(
      GetOpts().ParseArgs(args, missingArgIndex, missingArgCount,
                          includedFlagsBitmask, excludedFlagsBitmask));
  assert(ial && "No input argument list.");
  // Check for missing argument error.
  if (missingArgCount) {
    GetLangContext().GetDiagEngine().PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        diag::LLVMStr(ial->getArgString(missingArgIndex)),
        diag::UInt(missingArgCount));
    return nullptr;
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : ial->filtered(opts::UNKNOWN)) {
    GetLangContext().GetDiagUnit().PrintD(
        SrcLoc(), diag::err_unknown_arg, diag::LLVMStr(arg->getAsString(*ial)));
    return nullptr;
  }
  return ial;

}
