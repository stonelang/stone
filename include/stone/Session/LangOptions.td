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


}