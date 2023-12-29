// Include the common option parsing interfaces.
include "llvm/Option/OptParser.td"

/////////
// Flags

// The option should be accepted by cram 
def CompilerOption : OptionFlag;

// The option should not be accepted by the build.
def NoCompilerOption : OptionFlag;

// The option should not be accepted by the build.
def DriverOption : OptionFlag;

// The option should not be accepted by the build.
def NoDriverOption : OptionFlag;

// The option should not be accepted by the build.
def DebugOption : OptionFlag;

// The option's argument is a file-system path that may be affected by the
// current working directory.
def ArgumentIsPath : OptionFlag;

// The option should be written into a .stoneinterface module interface file,
// and read/parsed from there when reconstituting a .stonemodule from it.
def ModuleInterfaceOption : OptionFlag;

// The option should be written into a .stoneinterface module interface file,
// and read/parsed from there when reconstituting a .stonemodule from it.
// The option can be safely ignored by the older compiler.
def ModuleInterfaceOptionIgnorable : OptionFlag;

// The option causes the output of a supplementary output, or is the path option
// for a supplementary output. E.g., `-emit-module` and `-emit-module-path`.
def SupplementaryOutput : OptionFlag;

// The option should be accepted by stone-api-extract.
def StoneAPIExtractOption : OptionFlag;

// The option should be accepted by stone-symbolgraph-extract.
def StoneSymbolGraphExtractOption : OptionFlag;

// The option should be accepted by stone-api-digester.
def StoneAPIDigesterOption : OptionFlag;

def NoBatchOption : OptionFlag;

// The option should not force a full rebuild if added, changed, or removed.
def DoesNotAffectIncrementalBuild : OptionFlag;

def NoInteractiveOption : OptionFlag;

def ModeGroup : OptionGroup<"<mode options>">, HelpText<"MODES">;

class ModeOpt : Group<ModeGroup>;

def InternalGroup : OptionGroup<"<stone internal options>">;
def InternalDebugGroup :
OptionGroup<"<stone debug/development internal options>">,
Group<InternalGroup>, HelpText<"DEBUG/DEVELOPMENT OPTIONS">;

class InternalDebugOpt : Group<InternalDebugGroup>,
Flags<[HelpHidden]>;

//COMPILE ACTIONS

def Parse: Flag<["-"], "parse">,
HelpText<"Parse input file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def ResolveImports: Flag<["-"], "resolve-imports">,
HelpText<"Parse input file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;



def DumpAST : Flag<["-"], "dump-ast">,
HelpText<"Parse and dump ast tree">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def TypeCheck : Flag<["-"], "type-check">,
HelpText<"Parse and type-check input file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;


def PrintAST : Flag<["-"], "print-ast">,
HelpText<"Parse, type-check input file(s) and pretty print ast">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def EmitLibrary : Flag<["-"], "emit-library">,
HelpText<"Emit a linked library">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def Static : Flag<["-"], "static">,
  Flags<[CompilerOption, DriverOption]>,
  HelpText<"Make this module statically linkable and make the output of -emit-library a static library.">;


def EmitObject : Flag<["-"], "emit-object">,
HelpText<"Emit object file(s) (-c)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def c : Flag<["-"], "c">, Alias<EmitObject>,
Flags<[CompilerOption, DriverOption]>, ModeOpt;


def EmitModule : Flag<["-"], "emit-module">,
HelpText<"Emit module file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def EmitAssembly : Flag<["-"], "emit-assembly">,
HelpText<"Emit assembly file(s) (-s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def s: Flag<["-"], "S">, Alias<EmitAssembly>,
Flags<[CompilerOption, DriverOption]>, ModeOpt;

def EmitBC : Flag<["-"], "emit-bc">,
HelpText<"Emit LLVM BC file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def PrintIR : Flag<["-"], "print-ir">,
HelpText<"Print LLVM IR file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def EmitIRBefore : Flag<["-"], "emit-ir-before">,
HelpText<"Emit LLVM IR file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def EmitIRAfter : Flag<["-"], "emit-ir-after">,
HelpText<"Emit LLVM IR file(s)">, ModeOpt,
Flags<[CompilerOption, DriverOption]>;

def PrintHelp : Flag<["-", "--"], "help">, 
Flags<[CompilerOption, DriverOption]>,
HelpText<"Display available options">, ModeOpt;

def h : Flag<["-"], "h">, Alias<PrintHelp>;

def HelpSecrete : Flag<["-", "--"], "help-hidden">,
Flags<[CompilerOption]>,
HelpText<"Display available options, including hidden options">;


def PrintVersion : Flag<["-", "--"], "version">,
Flags<[CompilerOption, DriverOption]>,
HelpText<"Print version information and exit">, ModeOpt;

def v : Flag<["-"], "v">, 
Flags<[CompilerOption, DriverOption]>,
HelpText<"Show commands to run and use verbose output">;

def Lang : Flag<["-"], "st">, 
Flags<[CompilerOption, DriverOption]>,
HelpText<"Compiles only and skips the linking phase.">;


def SCPathEQ : Joined<["--"], "sc-path=">, 
Flags<[DebugOption, DriverOption]>,
HelpText<"Override the default paths for 'stone-compile'">;


def o : Flag<["-"], "o">,
HelpText<"Specifies a library which should be linked against">, ModeOpt,
Flags<[DriverOption]>;

def L : Flag<["-"], "L">,
HelpText<"Specifies a path to search for libraries">, ModeOpt,
Flags<[DriverOption]>;

def I : Flag<["-"], "I">,
HelpText<"Specifies a path to search for libraries">, ModeOpt,
Flags<[DriverOption]>;


def ModuleName : Separate<["-"], "module-name">,
  Flags<[CompilerOption, ModuleInterfaceOption, StoneAPIExtractOption,
         StoneSymbolGraphExtractOption]>,
  HelpText<"Name of the module to build">;

def ModuleNameEQ : Joined<["-"], "module-name=">, Flags<[CompilerOption]>,
  Alias<ModuleName>;

def ModuleAlias : Separate<["-"], "module-alias">,
  Flags<[CompilerOption, ModuleInterfaceOption]>,
  MetaVarName<"<alias_name=underlying_name>">,
  HelpText<"If a source file imports or references module <alias_name>, the <underlying_name> is used for the contents of the file">;

def ModuleLinkName : Separate<["-"], "module-link-name">,
  Flags<[CompilerOption, ModuleInterfaceOption]>,
  HelpText<"Library to link against when using this module">;


def ModuleLinkNameEQ : Joined<["-"], "module-link-name=">,
  Flags<[CompilerOption]>, Alias<ModuleLinkName>;


//GENERAL OPTIONS 
def Target : Separate<["-"], "target">,
Flags<[CompilerOption, DriverOption]>,
HelpText<"Generate code for the given target">;

def TargetVariant : Separate<["-"], "target-variant">,
  Flags<[CompilerOption, DriverOption]>,
  HelpText<"Generate 'zippered' code for macCatalyst that can run on the specified"
           " variant target triple in addition to the main -target triple">;

def TargetCPU : Separate<["-"], "target-cpu">, Flags<[CompilerOption, DriverOption]>,
HelpText<"Generate code for a particular CPU variant">;

// DEV OPTIONS 

def SyncJob : Flag<["-"], "sync-job">,
Flags<[DebugOption]>,
HelpText<"Run jobs synchronously -- default is asynchronous">;

def PrintMode : Flag<["-"], "print-mode">,
Flags<[DebugOption]>,
HelpText<"Print the current 'Compile' or 'Driver' mode">;

def PrintTime : Flag<["-"], "print-time">,
Flags<[DebugOption]>,
HelpText<"Print the time of the 'Compile' or 'Driver' lifecycle">;

def PrintStats : Flag<["-"], "print-stats">,
Flags<[DebugOption]>,
HelpText<"Print the 'Compile' statistics">;

def PrintDriverRequests : Flag<["-"], "print-requests">,
Flags<[DebugOption, DriverOption]>,
HelpText<"Print what the driver intends to do">;

def PrintDriverJobs : Flag<["-"], "print-jobs">,
Flags<[DebugOption, DriverOption]>,
HelpText<"Print the list of jobs to be executed">;

def PrintDriverLifecycle : Flag<["-"], "print-lifecycle">,
Flags<[DebugOption, DriverOption]>,
HelpText<"Print the lifecycle of activities and jobs">;

def ParseAsLibrary : Flag<["-"], "parse-as-library">,
  Flags<[CompilerOption]>,
  HelpText<"Parse the input file(s) as libraries, not scripts">;

def MainFile : Separate<["-"], "main-file">,
  HelpText<"Produce output for this file, not the whole module">;

def WorkDir : Separate<["-"], "work-dir">,
  HelpText<"Resolve file paths relative to the specified directory">,
  MetaVarName<"<path>">;

def IndexStorePath : Separate<["-"], "index-store-path">,
  Flags<[CompilerOption, ArgumentIsPath]>, MetaVarName<"<path>">,
  HelpText<"Store indexing data to <path>">;

def IndexUnitOutputPath : Separate<["-"], "index-unit-output-path">,
  Flags<[CompilerOption, ArgumentIsPath]>, MetaVarName<"<path>">,
  HelpText<"Use <path> as the output path in the produced index data.">;

def EmitLoadedModuleTracePath : Separate<["-"], "emit-loaded-module-trace-path">,
  Flags<[CompilerOption, ArgumentIsPath,
         SupplementaryOutput]>,
  HelpText<"Emit the loaded module trace JSON to <path>">,
  MetaVarName<"<path>">;

def EmitLoadedModuleTracePathEQ : Joined<["-"], "emit-loaded-module-trace-path=">,
  Flags<[CompilerOption, ArgumentIsPath,
         SupplementaryOutput]>,
  Alias<EmitLoadedModuleTracePath>;

def EmitModuleSummary :
  Flag<["-"], "emit-module-summary">,
  Flags<[NoInteractiveOption, SupplementaryOutput]>,
  HelpText<"Output module summary file">;

def EmitModuleSummaryPath :
  Separate<["-"], "emit-module-summary-path">,
  Flags<[CompilerOption, NoInteractiveOption, ArgumentIsPath,
         SupplementaryOutput]>,
  MetaVarName<"<path>">, HelpText<"Output module summary file to <path>">;

def SerializeDiagnostics : Flag<["-"], "serialize-diagnostics">,
  Flags<[CompilerOption, NoInteractiveOption, SupplementaryOutput]>,
  HelpText<"Serialize diagnostics in a binary format">;

def SerializeDiagnosticsPath : Separate<["-"], "serialize-diagnostics-path">,
  Flags<[CompilerOption, StoneAPIDigesterOption, NoBatchOption,
         ArgumentIsPath, SupplementaryOutput]>,
  HelpText<"Emit a serialized diagnostics file to <path>">,
  MetaVarName<"<path>">;

def SerializeDiagnosticsPathEQ: Joined<["-"], "serialize-diagnostics-path=">,
  Flags<[CompilerOption, StoneAPIDigesterOption, NoBatchOption,
         ArgumentIsPath, SupplementaryOutput]>,
  Alias<SerializeDiagnosticsPath>;

def EmitTBD : Flag<["-"], "emit-tbd">,
  HelpText<"Emit a TBD file">,
  Flags<[CompilerOption, NoInteractiveOption, SupplementaryOutput]>;
def EmitTBDPath : Separate<["-"], "emit-tbd-path">,
  Flags<[CompilerOption, NoInteractiveOption, ArgumentIsPath,
         SupplementaryOutput]>,
  HelpText<"Emit the TBD file to <path>">,
  MetaVarName<"<path>">;
def EmitTBDPathEQ : Joined<["-"], "emit-tbd-path=">,
  Flags<[CompilerOption, NoInteractiveOption, ArgumentIsPath,
         SupplementaryOutput]>,
  Alias<EmitTBDPath>;

def EmbedTBDForModule : Separate<["-"], "embed-tbd-for-module">,
  Flags<[CompilerOption]>,
  HelpText<"Embed symbols from the module in the emitted tbd file">;

def ColorDiagnostics : Flag<["-"], "color-diagnostics">,
  Flags<[CompilerOption, DoesNotAffectIncrementalBuild]>,
  HelpText<"Print diagnostics in color">;

def NoColorDiagnostics : Flag<["-"], "no-color-diagnostics">,
  Flags<[CompilerOption, DoesNotAffectIncrementalBuild]>,
  HelpText<"Do not print diagnostics in color">;

def DebugDiagnosticNames : Flag<["-"], "debug-diagnostic-names">,
  Flags<[CompilerOption, DoesNotAffectIncrementalBuild, HelpHidden]>,
  HelpText<"Include diagnostic names when printing">;

def ToolsDirectory : Separate<["-"], "tools-directory">,
  Flags<[CompilerOption,DoesNotAffectIncrementalBuild, ArgumentIsPath]>,
  HelpText<"Look for external executables (ld, clang, binutils) in <directory>">, MetaVarName<"<directory>">;


include "CompilerOptions.td"
