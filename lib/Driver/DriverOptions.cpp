#include "stone/Driver/DriverOptions.h"
#include "stone/Basic/Error.h"
#include "stone/Driver/Driver.h"

#include "llvm/Option/Arg.h"

using namespace stone;
using namespace llvm::opt;

// TODO: There is a potential to add duplicate files here.
static void AddInputFile(llvm::StringRef name, file::Type ty, unsigned fileID,
                         DriverOptions &driverOpts) {
  driverOpts.inputFiles.push_back(file::File(name, ty, fileID));
}

static void AddInputFile(llvm::StringRef name, unsigned fileID,
                         DriverOptions &driverOpts) {
  auto ty = file::GetTypeByName(name);
  assert(ty != file::Type::INVALID && "Invalid file type.");
  AddInputFile(name, ty, fileID, driverOpts);
}

file::Files &Driver::BuildInputFiles(const llvm::opt::InputArgList &ial) {
  llvm::DenseMap<llvm::StringRef, llvm::StringRef> seenFiles;
  unsigned fileID = 0;
  for (Arg *arg : ial) {
    if (arg->getOption().getKind() == llvm::opt::Option::InputClass) {
      auto input = arg->getValue();
      if (file::Exists(input)) {
        fileID++;
        auto fileType = file::GetTypeByExt(file::GetExt(input));
        switch (fileType) {
        case file::Type::Stone: {
          if (GetDriverOptions().inputFileType == file::Type::None) {
            GetDriverOptions().inputFileType = file::Type::Stone;
          } else if (GetDriverOptions().inputFileType != file::Type::Stone) {
            stone::Panic("Different file types"); // TODO: PrintD
          }
          AddInputFile(input, fileType, fileID, GetDriverOptions());
          break;
        }
        case file::Type::Object: {
          if (GetDriverOptions().inputFileType == file::Type::None) {
            GetDriverOptions().inputFileType = file::Type::Object;
          } else if (GetDriverOptions().inputFileType != file::Type::Object) {
            // TODO: Different file types
            stone::Panic("Different file types"); // TODO: PrintD
          }
          AddInputFile(input, fileType, fileID, GetDriverOptions());
          break;
        }
        default:
          stone::Panic("Unknown file type");
          break;
        }
        if (fileType == file::Type::Stone) {
          auto baseName = llvm::sys::path::filename(input);

          if (!seenFiles.insert({baseName, input}).second) {
            stone::Panic("error_two_files_same_name");

            // GetContext().Out()
            //     << "de.D(SourceLoc(),"
            //     << "diag::error_two_files_same_name,"
            //     << "basename, seenFiles[basename], argValue);" << '\n';
            // GetContext().Out() << " de.D(SourceLoc(), "
            //                  << "diag::note_explain_two_files_"
            //                     "same_name);"
            //                  << '\n';
          }
        }
      }
    }
  }
  return GetDriverOptions().inputFiles;
}

static Error ComputeDriverOptions(llvm::opt::InputArgList &ial,
                                  DriverOptions &driverOpts) {

  // Since the mode has already been created
  // switch(GetMode().GetKind().)
  driverOpts.outputFileType = file::Type::Object;

  // TODO:
  // GetDriverOptions().compileModel = ComputeCompilationMode(
  //     *tal, GetDriverOptions().inputFiles);

  // auto scPathResult = GetEQValue(opts::LangPathEQ);
  // if (!stPathResult.IsErr()) {
  //   GetDriverOptions().scPath = stPathResult.Get();
  // }

  driverOpts.printRequests = ial.hasArg(opts::PrintDriverRequests);
  driverOpts.printJobs = ial.hasArg(opts::PrintDriverJobs);
  driverOpts.printLifecycle = ial.hasArg(opts::PrintDriverLifecycle);
  driverOpts.systemOpts.printStatistics = ial.hasArg(opts::PrintStats);

  return Error();
}

Error Driver::ComputeOptions(llvm::opt::InputArgList &ial) {

  driverOpts = std::make_unique<DriverOptions>(Mode::Create(ial));
  if (driverOpts->GetMode().IsAlien()) {
    return Error(true);
  }
  if (ComputeDriverOptions(ial, *driverOpts.get()).Has()) {
    return Error(true);
  }
  return Error();
}
