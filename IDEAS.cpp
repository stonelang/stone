

/// Whenever you build a CompileJobConstructoin -- perform notificaton

class CompileInvocationConsumer {
  llvm::SmallVector<CompilationEntity> entities;

public:
  virtual void CompletedStoneFileType(CompileJobConstruction *cjc);
  virtual void CompletedObjectFileType(DriverInputFile *dif);
  virtual void CompleteAutoLinkFileType(DriverInputFile *dif);
  virtual void CompletedStoneModuleFileType(DriverInputFile *dif);
}

class ShouldLinkCompileInvocationConsumer : public CompileInvocationConsumer {

}

class ShouldMergeModuleCmpileInvocationConsumer
    : public CompileInvocationConsumer {

}

class CompleteCompileInvocationConsumer : public CompileInvocationConsumer {

}

// class CompletedFileTypeConsumer

class ShoulWrapCompileInvocationConsumer {

}

class CompileInvocation {
  llvm::SmallVector<CompileInvocationConsumer> consumers;

public:
  virtual void ActOnStoneFileType();
  virtual void ActOnObjectFileType();
  virtual void ActOnAutoLinkFileType();
  virtual void ActOnStoneModuleFileType();

}

class MultipleCompileInvocation : public CompileInvocation {
};

auto compileInvocation = std::make_unique<CompileInvocation>();
compileInvocation.AddConsumer(...) compileInvocation->ActOnInputFileTypes();

class DriverImplementaion {

public:
  virtual void MergeModule();

  bool ShouldAutolinkExtract();
  bool ShouldMergeModule();
  bool ShouldPerformLTO();
  bool ShouldLink();

public:
  virtual void ActOnStoneFileType();
  virtual void ActOnObjectFileType();
  virtual void ActOnAutoLinkFileType();
  virtual void ActOnStoneModuleFileType();

public:
public:
  virtual void BuildJobConstructions() = 0;
}

class MultipleCompileInvocation : public DriverImplementaion {

public:
  void ActOnStoneFileType(const DriverInputFile *input) override {}
  void ActOnObjectFileType() override {}
  void ActOnAutoLinkFileType() override {}
  void ActOnStoneModuleFileType() override {}

}

void MultipleCompileInvocation::BuildJobConstructions() {

  auto fileTypeExectuion = GetFileTypeExecution() switch (GetFileType()) {

  case FileType::Stone: {
    WithLink() { WithCompile() }
  }
  case FileType::Object: {
    WithLink();
  }
  }
}

void MultipleCompileInvocation::ActOnStoneFileType(
    const DriverInputFile *input) {
  // create compilejc
}

class BuildingCompilation final {
public:
  CompilationEntityList moduleInputs;
  CompilationEntityList linkerInputs;

public:
  BuildingJobConstructions(Driver &driver) : driver(driver) {}
  ~BuildingJobConstructions();

public:
  bool HasModuleInputs() {
    return !moduleInputs.empty() && moduleInputs.size() > 0;
  }
  bool HasLinkInputs() {
    return !linkerInputs.empty() && linkerInputs.size() > 0;
  }

public:
  void AddModuleInput(const CompilationEntity *entity);
  void AddLinkInput(const CompilationEntity *entity);

public:
  Status MergeModule();
  bool ShouldLink() { return (driver.ShouldLink() && HasLinkInputs()); }
  bool ShouldModuleWrap();
  bool ShouldPerformLTO() {
    driver.GetDriverOptions().GetDriverOutputInfo().HasLTO();
  }
  bool ShouldAutolinkExtract() {}
};

class MultipleCompileInvocation final : BuildingCompilation {

public:
  CompilationEntityList *BuildJobConstructions() override;

public:
  void BuildForStoneFileType(const DriverInputFile *input) override {}
  void BuildForObjectFileType() override {}
  void BuildForAutoLinkFileType() override {}
  void BuildForStoneModuleFileType() override {}
};

class SingleCompileInvocation final : BuildingJobConstructions {
public:
  CompilationEntityList *BuildJobConstructions() override;
};

static void BuildMultipleCompileInvocation(
    BuildingJobConstructions &buildingJobConstructions) {}

static void
SingleCompileInvocation(BuildingJobConstructions &buildingJobConstructions) {}

static CompilationEntityList *
FinishJobConstructions(BuildingJobConstructions &buildingJobConstructions) {}

static void
BuildJobConstructions(BuildingJobConstructions &buildingJobConstructions) {

  switch (GetCompileInvocationMode()) {
  case CompileInvocationMode::Multiple:
    return BuildMultipleCompileInvocation(buildingJobConstructions);
  case CompileInvocationMode:
  Single:
    retun BuildSingleCompileInvocation(buildingJobConstructions);
  }
  llvm_unreachable(".....");
}
CompilationEntityList *Driver::BuildJobConstructions() {

  BuildingJobConstructions buildingJobConstructions(*this);
  BuildJobConstructions(buildingJobConstructions);
  return FinishJobConstructions(BuildingJobConstructions);
}

class CompileInvocationConsumer {
public:
  llvm::SmallVector<CompilationEntity> entities;

protected:
  void AddCompilationEntity(const CompilationEntity *entity) {
    entities.push_back(entity);
  }

public:
  virtual void CompletedCompilationEntity(const CompilationEntity *entity) = 0;

}

class ModuleInputs final : public CompileInvocationConsumer {
};

class LinkerInputs final : public CompileInvocationConsumer {};

class CompilationInvocation {

protected:
  Driver &driver;

public:
  CompilationInvocation(Driver &driver) : driver(driver) {}

public:
  virtual void BuildJobConstructions() = 0;
};

class MultipleCompilationInvocation final : public CompilationInvocation {

public:
  MultipleCompilationInvocation(Driver &driver)
      : CompilationInvocation(driver) {}

public:
  Status ActOnStoneFileType(const DriverInputFile *input = nullptr);
  Status ActOnObjectFileType(const DriverInputFile *input);
  Status ActOnAutoLinkFileType(const DriverInputFile *input);
  Status ActOnStoneModuleFileType(const DriverInputFile *input);
}
