#ifndef STONE_SYNTAX_DECLSPECIFIER_H
#define STONE_SYNTAX_DECLSPECIFIER_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {



class ScopeSpecifier {

};

class Declarator {
public:
};

// /// A context for parsing declaration specifiers.  TODO: flesh this
// /// out, there are other significant restrictions on specifiers than
// /// would be best implemented in the parser.
// enum class DeclSpecifierContext {
//   Normal,         // normal context
//   Fun,
//   Struct,          // struct context, enables 'friend'
//   Type, // C++ type-specifier-seq or C specifier-qualifier-list
//   Trailing, // C++11 trailing-type-specifier in a trailing return type
//   AliasDeclaration,  // C++11 type-specifier-seq in an alias-declaration
//   TopLevel,          // top-level/namespace declaration context
//   TemplateParam,     // template parameter context
//   TemplateTypeArg,  // template type argument context
//   Condition           // condition declaration context
// };

class DeclSpecifier {
public:
};

} // namespace syn
} // namespace stone
#endif
