let Flags = [LangOption, NoDriverOption] in {

def PrimaryFile : Separate<["-"], "primary-file">,
  HelpText<"Produce output for this file, not the whole module">;

def FileList : Separate<["-"], "filelist">,
  HelpText<"Specify source inputs in a file rather than on the command line">;

def PrimaryFileList : Separate<["-"], "primary-filelist">,
  HelpText<"Specify primary inputs in a file rather than on the command line">;

def OutputFileList : Separate<["-"], "output-filelist">,
  HelpText<"Specify outputs in a file rather than on the command line">;

def SupplementaryOutputFileMap : Separate<["-"], "supplementary-output-file-map">,
  HelpText<"Specify supplementary outputs in a file rather than on the command line">;

def BadFileDescriptorRetryCount:
  Separate<["-"], "bad-file-descriptor-retry-count">,
  HelpText<"Number of retrying opening a file if previous open returns a bad "
           "file descriptor error.">;

def BypassBatchModeChecks: Flag<["-"], "bypass-batch-mode-checks">,
    HelpText<"Bypass checks for batch-mode errors.">;

def IndexUnitOutputPathFileList : Separate<["-"], "index-unit-output-path-filelist">,
  HelpText<"Specify index unit output paths in a file rather than on the command line">;

def EmitDependenciesPath
  : Separate<["-"], "emit-dependencies-path">, MetaVarName<"<path>">,
    HelpText<"Output basic Make-compatible dependencies file to <path>">;

def EmitReferenceDependencies : Flag<["-"], "emit-reference-dependencies">,
  HelpText<"Emit a Swift-style dependencies file">;

def EmitReferenceDependenciesPath
  : Separate<["-"], "emit-reference-dependencies-path">, MetaVarName<"<path>">,
    HelpText<"Output Swift-style dependencies file to <path>">;

def EmitFixitsPath
  : Separate<["-"], "emit-fixits-path">, MetaVarName<"<path>">,
    HelpText<"Output compiler fixits as source edits to <path>">;

def EmitABIDescriptorPath
  : Separate<["-"], "emit-abi-descriptor-path">, MetaVarName<"<path>">,
    HelpText<"Output the ABI descriptor of current module to <path>">;

def EmitModuleSemanticInfoPath
  : Separate<["-"], "emit-module-semantic-info-path">, MetaVarName<"<path>">,
    HelpText<"Output semantic info of current module to <path>">;





}