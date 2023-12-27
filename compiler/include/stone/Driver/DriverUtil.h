#ifndef SWIFT_DRIVER_DRIVERUTIL_H
#define SWIFT_DRIVER_DRIVERUTIL_H

#include "stone/Basic/LLVM.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/StringSaver.h"

#include <memory>

namespace stone {

class DiagnosticEngine;
/// Expand response files in the argument list with retrying.
/// This function is a wrapper of lvm::cl::ExpandResponseFiles. It will
/// retry calling the function if the previous expansion failed.
void ExpandResponseFilesWithRetry(llvm::StringSaver &Saver,
                                  llvm::SmallVectorImpl<const char *> &Args);

/// Generates the list of arguments that would be passed to the compiler
/// invocation from the given driver arguments.
///
/// \param ArgList The driver arguments (i.e. normal arguments for \c stonec).
/// \param Diags The DiagnosticEngine used to report any errors parsing the
/// arguments.
/// \param JobAction Called with the list of invocation arguments if there were
/// no errors in processing \p ArgList. This is a callback rather than a return
/// value to avoid copying the arguments more than necessary.
/// \param ForceNoOutputs If true, override the output mode to "-typecheck" and
/// produce no outputs. For example, this disables "-emit-module" and "-c" and
/// prevents the creation of temporary files.
///
/// \returns True on error, or if \p JobAction returns true.
///
/// \note This function is not intended to create invocations which are
/// suitable for use in REPL or immediate modes.
bool GetSingleCompilerConfigurationFromDriverArguments(
    ArrayRef<const char *> ArgList, DiagnosticEngine &Diags,
    llvm::function_ref<bool(ArrayRef<const char *> CompilerArgs)> JobAction,
    bool ForceNoOutputs = false);

} // end namespace stone

#endif
