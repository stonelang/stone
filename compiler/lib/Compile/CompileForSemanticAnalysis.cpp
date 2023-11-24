#include "stone/Compile/CompilerInstance.h"

using namespace stone;

/// Handles only syntax
// Status CompilerInstance::CompileForSemanticAnalysis() {}

// Status CompilerInstance::CompileForTypeCheck(
//     std::function<Status(syn::SyntaxFile &)> notifiy) {

//   if (CompileForResolveImports().IsError()) {
//     return Status::Error();
//   }
//   // ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
//   //                       TypeCheckerOptions &typeCheckerOpts,
//   //                       stone::TypeCheckerListener *listener) {

//   //   stone::TypeCheckSyntaxFile(syntaxFile, typeCheckerOpts, listener);

//   // });

//   return Status();
// }

// /// Handles semantics
// Status CompilerInstance::CompileForPrintAST() {
//   // CompileForTypeCheck();
// }