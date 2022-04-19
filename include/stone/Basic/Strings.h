#ifndef STONE_BASIC_STRINGS_H
#define STONE_BASIC_STRINGS_H

#include <string>

namespace stone {
// TODO: Strings.def
namespace strings {

/// Substitute for <stdout>
constexpr static const char *Dash = "-";

// The function name of the main program
constexpr static const char *MainFunctionName = "Main";

// TODO: This may be the default name for the module
constexpr static const char *MainFileName = "main";

///
constexpr static const char *StdLibName = "Stone";

///
constexpr static const char *BuiltinName = "Builtin";

///
constexpr static const char *BuiltinInt32Name = "Builtin.Int";

} // namespace strings
} // namespace stone
#endif
