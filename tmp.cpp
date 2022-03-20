

class Compilation {};

class FileTypeCompilation {
  Compilation &compilation;

public:
  FileTypeCompilation(Compilation &compilation) : compilation(compilation) {}

public:
  Request *MakeInputRequest() = 0;
  Request *MakeCompileRequest() = 0;
  Request *MakeLinkRequest() = 0;

  virtual void BuildMultiCompilingModel() {}
  virtual void BuildCompileRequest();
  virtual void BuildLinkRequest();
};

class StoneFileTypeCompilation : public FileTypeCompilation {

public:
  StoneFileTypeCompilation(Compilation &compilation)
      : FileTypeCompilation(compilation) {}

  void BuildMultiCompilingModel() override {
    for (auto &input : inputs) {
      // TODO: Way out there, but there is potential for git here?
      if (comp.GetDriver().GetBuildSystem().IsDirty(input)) {

        assert(input.GetType() == GetInputFileType() &&
               "Incompatible input file types");
        assert(file::IsPartOfCompilation(input.GetType()));

        hc.currentRequest = MakeInputRequest(input);
        hc.currentRequest = MakeCompileRequest(request, GetOutputFileType());
        hc.AddModuleInput(hc.currentRequest);
      }
    }
  }
  void BuildMultiCompilingModel(Request *request) override {

    // Create a single CompileJobRequest to handl all InputRequest(s)
    auto *compileRequest = MakeRequest<CompileJobRequest>(GetOutputFileType());
    for (auto &input : inputs) {

      if (GetBuildSystem().IsDirty(input)) {
        assert(input.GetType() == GetInputFileType() &&
               "Incompatible input file types");

        assert(file::IsPartOfCompilation(input.GetType()));

        compileRequest->AddInput(MakeRequest<InputRequest>(input));

        compilingModel.BuildRequest()

            if (CompilingModelKind::Single) {
          hc.AddModuleInput(compileRequest);
          if (outputOptions.CanLink()) {
            hc.AddLinkInput(hc.currentRequest);
          }
        }
      }
    }
  }
  void BuildCompilationModel() {}
};

// CompilingModel ComputeCompilingModel(Compilation & compilation){

// }
// CompilingModel.BuildCompilation();

void StoneFileTypeCompilation::BuildCompilation() {
  auto compilingMode = GetCompilingModel(GetCompilingModelKind());

  for (auto &input : inputs) {

    if (GetBuildSystem().IsDirty(input)) {
      assert(input.GetType() == GetInputFileType() &&
             "Incompatible input file types");

      assert(file::IsPartOfCompilation(input.GetType()));

      compilingModel.BuildRequest(file);
    }
  }

  int main() {

    Compilation compilation;
    StoneFileTypeCompilation stoneFileTypeCompilation(compilation);
    stoneFileTypeCompilation.BuildMultiCompilingModel();

    Compilation compilation;
    StoneFileTypeCompilation stoneFileTypeCompilation(compilation);
    stoneFileTypeCompilation.BuildCompilation();

    return 0;
  }