// Include the common option parsing interfaces.
include "llvm/Option/OptParser.td"

/////////
// Flags

// The option should be accepted by cram 
def FrontendOption : OptionFlag;

// The option should not be accepted by the build.
def NoFrontendOption : OptionFlag;

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
Flags<[FrontendOption]>;


def DumpSyntax : Flag<["-"], "dump-syntax">,
HelpText<"Parse and dump syntax tree">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def TypeCheck : Flag<["-"], "type-check">,
HelpText<"Parse and type-check input file(s)">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;


def PrintSyntax : Flag<["-"], "print-syntax">,
HelpText<"Parse, type-check input file(s) and pretty print syntax tree">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def EmitLibrary : Flag<["-"], "emit-library">,
HelpText<"Emit a linked library">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def Static : Flag<["-"], "static">,
  Flags<[FrontendOption, DriverOption]>,
  HelpText<"Make this module statically linkable and make the output of -emit-library a static library.">;


def EmitObject : Flag<["-"], "emit-object">,
HelpText<"Emit object file(s) (-c)">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def c : Flag<["-"], "c">, Alias<EmitObject>,
Flags<[FrontendOption, DriverOption]>, ModeOpt;


def EmitModule : Flag<["-"], "emit-module">,
HelpText<"Emit module file(s)">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def EmitAssembly : Flag<["-"], "emit-assembly">,
HelpText<"Emit assembly file(s) (-s)">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def s: Flag<["-"], "S">, Alias<EmitAssembly>,
Flags<[FrontendOption, DriverOption]>, ModeOpt;

def EmitBC : Flag<["-"], "emit-bc">,
HelpText<"Emit LLVM BC file(s)">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def EmitIR : Flag<["-"], "emit-ir">,
HelpText<"Emit LLVM IR file(s)">, ModeOpt,
Flags<[FrontendOption, DriverOption]>;

def PrintHelp : Flag<["-", "--"], "help">, 
Flags<[FrontendOption, DriverOption]>,
HelpText<"Display available options">, ModeOpt;

def h : Flag<["-"], "h">, Alias<PrintHelp>;

def HelpSecrete : Flag<["-", "--"], "help-hidden">,
Flags<[FrontendOption]>,
HelpText<"Display available options, including hidden options">;


def PrintVersion : Flag<["-", "--"], "version">,
Flags<[FrontendOption, DriverOption]>,
HelpText<"Print version information and exit">, ModeOpt;

def v : Flag<["-"], "v">, 
Flags<[FrontendOption, DriverOption]>,
HelpText<"Show commands to run and use verbose output">;

def Lang : Flag<["-"], "st">, 
Flags<[FrontendOption, DriverOption]>,
HelpText<"Compiles only and skips the linking phase.">;


def LangPathEQ : Joined<["--"], "sc-path=">, 
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
  Flags<[FrontendOption, ModuleInterfaceOption, StoneAPIExtractOption,
         StoneSymbolGraphExtractOption]>,
  HelpText<"Name of the module to build">;

def ModuleNameEQ : Joined<["-"], "module-name=">, Flags<[FrontendOption]>,
  Alias<ModuleName>;

def ModuleAlias : Separate<["-"], "module-alias">,
  Flags<[FrontendOption, ModuleInterfaceOption]>,
  MetaVarName<"<alias_name=underlying_name>">,
  HelpText<"If a source file imports or references module <alias_name>, the <underlying_name> is used for the contents of the file">;

def ModuleLinkName : Separate<["-"], "module-link-name">,
  Flags<[FrontendOption, ModuleInterfaceOption]>,
  HelpText<"Library to link against when using this module">;


def ModuleLinkNameEQ : Joined<["-"], "module-link-name=">,
  Flags<[FrontendOption]>, Alias<ModuleLinkName>;


//GENERAL OPTIONS 
def Target : Separate<["-"], "target">,
Flags<[FrontendOption, DriverOption]>,
HelpText<"Generate code for the given target">;

def TargetVariant : Separate<["-"], "target-variant">,
  Flags<[FrontendOption, DriverOption]>,
  HelpText<"Generate 'zippered' code for macCatalyst that can run on the specified"
           " variant target triple in addition to the main -target triple">;

def TargetCPU : Separate<["-"], "target-cpu">, Flags<[FrontendOption, DriverOption]>,
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

def MainFile : Separate<["-"], "main-file">,
  HelpText<"Produce output for this file, not the whole module">;

def WorkDir : Separate<["-"], "work-dir">,
  HelpText<"Resolve file paths relative to the specified directory">,
  MetaVarName<"<path>">;

def IndexStorePath : Separate<["-"], "index-store-path">,
  Flags<[FrontendOption, ArgumentIsPath]>, MetaVarName<"<path>">,
  HelpText<"Store indexing data to <path>">;

def IndexUnitOutputPath : Separate<["-"], "index-unit-output-path">,
  Flags<[FrontendOption, ArgumentIsPath]>, MetaVarName<"<path>">,
  HelpText<"Use <path> as the output path in the produced index data.">;

def EmitLoadedModuleTracePath : Separate<["-"], "emit-loaded-module-trace-path">,
  Flags<[FrontendOption, ArgumentIsPath,
         SupplementaryOutput]>,
  HelpText<"Emit the loaded module trace JSON to <path>">,
  MetaVarName<"<path>">;

def EmitLoadedModuleTracePathEQ : Joined<["-"], "emit-loaded-module-trace-path=">,
  Flags<[FrontendOption, ArgumentIsPath,
         SupplementaryOutput]>,
  Alias<EmitLoadedModuleTracePath>;

def EmitModuleSummary :
  Flag<["-"], "emit-module-summary">,
  Flags<[NoInteractiveOption, SupplementaryOutput]>,
  HelpText<"Output module summary file">;

def EmitModuleSummaryPath :
  Separate<["-"], "emit-module-summary-path">,
  Flags<[FrontendOption, NoInteractiveOption, ArgumentIsPath,
         SupplementaryOutput]>,
  MetaVarName<"<path>">, HelpText<"Output module summary file to <path>">;

def SerializeDiagnostics : Flag<["-"], "serialize-diagnostics">,
  Flags<[FrontendOption, NoInteractiveOption, SupplementaryOutput]>,
  HelpText<"Serialize diagnostics in a binary format">;

def SerializeDiagnosticsPath : Separate<["-"], "serialize-diagnostics-path">,
  Flags<[FrontendOption, StoneAPIDigesterOption, NoBatchOption,
         ArgumentIsPath, SupplementaryOutput]>,
  HelpText<"Emit a serialized diagnostics file to <path>">,
  MetaVarName<"<path>">;

def SerializeDiagnosticsPathEQ: Joined<["-"], "serialize-diagnostics-path=">,
  Flags<[FrontendOption, StoneAPIDigesterOption, NoBatchOption,
         ArgumentIsPath, SupplementaryOutput]>,
  Alias<SerializeDiagnosticsPath>;

def EmitTBD : Flag<["-"], "emit-tbd">,
  HelpText<"Emit a TBD file">,
  Flags<[FrontendOption, NoInteractiveOption, SupplementaryOutput]>;
def EmitTBDPath : Separate<["-"], "emit-tbd-path">,
  Flags<[FrontendOption, NoInteractiveOption, ArgumentIsPath,
         SupplementaryOutput]>,
  HelpText<"Emit the TBD file to <path>">,
  MetaVarName<"<path>">;
def EmitTBDPathEQ : Joined<["-"], "emit-tbd-path=">,
  Flags<[FrontendOption, NoInteractiveOption, ArgumentIsPath,
         SupplementaryOutput]>,
  Alias<EmitTBDPath>;

def EmbedTBDForModule : Separate<["-"], "embed-tbd-for-module">,
  Flags<[FrontendOption]>,
  HelpText<"Embed symbols from the module in the emitted tbd file">;

def ColorDiagnostics : Flag<["-"], "color-diagnostics">,
  Flags<[FrontendOption, DoesNotAffectIncrementalBuild]>,
  HelpText<"Print diagnostics in color">;

def NoColorDiagnostics : Flag<["-"], "no-color-diagnostics">,
  Flags<[FrontendOption, DoesNotAffectIncrementalBuild]>,
  HelpText<"Do not print diagnostics in color">;

def DebugDiagnosticNames : Flag<["-"], "debug-diagnostic-names">,
  Flags<[FrontendOption, DoesNotAffectIncrementalBuild, HelpHidden]>,
  HelpText<"Include diagnostic names when printing">;


include "FrontendOptions.td"
