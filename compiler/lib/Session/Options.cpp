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

unsigned opts::GetArgID(llvm::opt::Arg *arg) {
  assert(arg);
  return arg->getOption().getID();
}

llvm::StringRef opts::GetArgName(llvm::opt::Arg *arg) {
  assert(arg);
  return arg->getOption().getName();
}




