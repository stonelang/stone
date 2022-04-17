#include "stone/Compile/LangInputsConverter.h"

using namespace stone;

LangInputsConverter::LangInputsConverter(DiagnosticEngine &de,
                                         const llvm::opt::ArgList &args)
    : de(de), args(args), fileListPathArg(args.getLastArg(opts::FileList)),
      primaryFileListPathArg(args.getLastArg(opts::PrimaryFileList)),
      badFileDescriptorRetryCountArg(
          args.getLastArg(opts::BadFileDescriptorRetryCount)) {}
