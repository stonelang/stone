// Include the common option parsing interfaces.
include "llvm/Option/OptParser.td"

/////////
// Flags

// The option should be accepted by cram 
def LangOption : OptionFlag;

// The option should not be accepted by the build.
def NoLangOption : OptionFlag;

// The option should not be accepted by the build.
def DriverOption : OptionFlag;

// The option should not be accepted by the build.
def NoDriverOption : OptionFlag;

// The option should not be accepted by the build.
def DebugOption : OptionFlag;

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
Flags<[LangOption]>;


def EmitParse : Flag<["-"], "emit-parse">,
HelpText<"Parse and type-check input file(s) and print Syntax(s)">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def TypeCheck : Flag<["-"], "type-check">,
HelpText<"Parse and type-check input file(s)">, ModeOpt,
Flags<[LangOption, DriverOption]>;


def EmitSyntax : Flag<["-"], "emit-syntax">,
HelpText<"Parse and type-check input file(s) and print Syntax(s)">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def EmitLibrary : Flag<["-"], "emit-library">,
HelpText<"Emit a linked library">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def Static : Flag<["-"], "static">,
  Flags<[LangOption, DriverOption]>,
  HelpText<"Make this module statically linkable and make the output of -emit-library a static library.">;


def EmitObject : Flag<["-"], "emit-object">,
HelpText<"Emit object file(s) (-c)">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def c : Flag<["-"], "c">, Alias<EmitObject>,
Flags<[LangOption, DriverOption]>, ModeOpt;


def EmitModule : Flag<["-"], "emit-module">,
HelpText<"Emit module file(s)">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def EmitAssembly : Flag<["-"], "emit-assembly">,
HelpText<"Emit assembly file(s) (-s)">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def s: Flag<["-"], "S">, Alias<EmitAssembly>,
Flags<[LangOption, DriverOption]>, ModeOpt;

def EmitBC : Flag<["-"], "emit-bc">,
HelpText<"Emit LLVM BC file(s)">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def EmitIR : Flag<["-"], "emit-ir">,
HelpText<"Emit LLVM IR file(s)">, ModeOpt,
Flags<[LangOption, DriverOption]>;

def PrintHelp : Flag<["-", "--"], "help">, 
Flags<[LangOption, DriverOption]>,
HelpText<"Display available options">, ModeOpt;

def h : Flag<["-"], "h">, Alias<PrintHelp>;

def HelpSecrete : Flag<["-", "--"], "help-hidden">,
Flags<[LangOption]>,
HelpText<"Display available options, including hidden options">;


def PrintVersion : Flag<["-", "--"], "version">,
Flags<[LangOption, DriverOption]>,
HelpText<"Print version information and exit">, ModeOpt;

def v : Flag<["-"], "v">, 
Flags<[LangOption, DriverOption]>,
HelpText<"Show commands to run and use verbose output">;

def Lang : Flag<["-"], "stc">, 
Flags<[LangOption, DriverOption]>,
HelpText<"Compiles only and skips the linking phase.">;


def LangPathEQ : Joined<["--"], "stc-path=">, 
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


//GENERAL OPTIONS 
def Target : Separate<["-"], "target">,
Flags<[LangOption, DriverOption]>,
HelpText<"Generate code for the given target">;

def TargetVariant : Separate<["-"], "target-variant">,
  Flags<[LangOption, DriverOption]>,
  HelpText<"Generate 'zippered' code for macCatalyst that can run on the specified"
           " variant target triple in addition to the main -target triple">;

def TargetCPU : Separate<["-"], "target-cpu">, Flags<[LangOption, DriverOption]>,
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


