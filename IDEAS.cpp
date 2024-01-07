class TopLevelJobEntitiesBuilder final {
  Driver &driver;
  llvm::SmallVector<TopLevelCompilationEntitiesConsumer *> consumers;

public:
  TopLevelJobEntitiesBuilder(Driver &driver);

public:
  Status BuildTopLevelJobEntities(TopLevelCompilationEntities &entities){
    entities.ForEachTopLevelJobConstruction([&](const CompilationEntity *entity) {
    if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
      BuildTopLevelJob(jc);
    }
  });
  }

  // Let us say this is a compile job 
  Job *BuildTopLevelJob(const JobConstruction *jc){


  }

public:
  void AddConsumer(TopLevelCompilationEntitiesConsumer *consumer);
  void Finish();
};

Status Driver::BuildTopLevelJobEntities(TopLevelCompilationEntities &entities) {

  STONE_DEFER { jobEntitiesBuilder.Finish(); };

  if (!entities.HasTopLevelJobConstructions()) {
    return Status::MakeHasCompletionAndIsError();
  }
  return jobEntitiesBuilder.BuildTopLevelJobEntities(entities);
  
}

// LinkJC -> LinkJB
//   CompileJC  -> CompileJB 
//      InputFile 
//      PCHJC 




class TopLevelEntitiesConsumer
    : public DriverAllocation<TopLevelEntitiesConsumer> {

protected:
  Driver &driver;

protected:
  llvm::SmallVector<const CompilationEntity *> entities;

public:
  TopLevelEntitiesConsumer(Driver &driver);

protected:
  void AddTopLevelEntity(const CompilationEntity *entity) {
    entities.push_back(entity);
  }
  bool HasTopLevelEntities() {
    return (entities.empty() && entities.size() > 0);
  }

public:
  TopLevelEntitiesConsumer();

public:
  virtual void CompletedEntity(const CompilationEntity *entity);
  virtual void Finish();
};


class LinkJCEntitiesConsumer final
    : public TopLevelCompilationEntitiesConsumer {
public:
  LinkJCEntitiesConsumer(Driver &driver);

public:
  void CompletedEntity(const CompilationEntity *entity) override;
  void Finish() override;

public:
  static LinkJCEntitiesConsumer *Create(Driver &driver);
};


class MergeModuleJCEntitiesConsumer final
    : public TopLevelEntitiesConsumer {

public:
  MergeModuleJCEntitiesConsumer(Driver &driver);

public:
  void CompletedEntity(const CompilationEntity *entity) override;
  void Finish() override;

public:
  static MergeModuleJCEntitiesConsumer *Create(Driver &driver);
};


class TopLevelEntitiesBuilder final {

  Driver &driver;
  llvm::SmallVector<TopLevelEntitiesConsumer *> consumers;

public:
  TopLevelEntitiesBuilder(Driver &driver);

public:
  Status BuildCompileInvocation(CompileInvocationMode cim);
  Status BuildMultipleCompileInvocation();
  Status BuildSingleCompileInvocation();
  Status BuildBatchCompileInvocation();

};

Status Driver::BuildTopLevelJobConstructionEntities(){
  
}
Status Driver::BuildMultipleCompileInvocation(BuildingCompilationEntitiesConsumer* consumer) {


  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        switch (input->GetFileType()) {
        case FileType::Stone: {
          assert(input->IsPartOfStoneCompilation());
          consumer->CompletedCompilationEntity(CreateCompileJobConstruction(input));
          break;
        }
        case FileType::Object: {
          consumer->CompletedCompilationEntity(CompletedCompilationEntity(input));
          break;
        }
        default:
          llvm_unreachable(" Invalid file type");
        }
      });

  consumer->CompletedCompilationEntity

}
Status Driver::BuildSingleCompileInvocation(BuildingCompilationEntitiesConsumer* consumer){


}
Status Driver::BuildBatchCompileInvocation(BuildingCompilationEntitiesConsumer* consumer);


Driver::BuildTopLevelJobConstructionEntities(TopLevelJobConstructionEntities& entities) {

 STONE_DEFER {
    [&]() {
      // Do something here
    }();
  };

  BuildMultipleCompileInvocation();
}


class BuildingCompilationEntitiesConsumer {
protected:
  Driver &driver;

protected:
  llvm::SmallVector<const CompilationEntity *> entities;

public:
  BuildingCompilationEntitiesConsumer(Driver &driver);

protected:
  void AddCompilationEntity(const CompilationEntity *entity) {
    entities.push_back(entity);
  }
  bool HasCompilationEntities() {
    return (entities.empty() && entities.size() > 0);
  }

public:
  BuildingCompilationEntitiesConsumer();
public:
  virtual void CompletedCompilationEntity(const CompilationEntity *entity);

};

class JobConstructionEntitiesConsumer final
    : public BuildingCompilationEntitiesConsumer {
public:
  JobConstructionEntitiesConsumer(Driver &driver);

public:
  void CompletedCompilationEntity(const CompilationEntity *entity) override {

      if(entity->HasIsTopLevel()){
        driver.AddTopLevelJobConstruction(entity);
      }else{
        AddCompilationEntity(entity);
      }
  }
};


// class JCBuilder final {

//   Driver &driver;
//   llvm::SmallVector<BuildingCompilationEntitiesConsumer *> consumers;
// public:
//   JCBuilder(Driver &driver);

// public:
//   Status BuildForMultipleCompileInvocation(){
//   }
//   Status BuildForSingleCompileInvocation();
//   Status BuildForBatchCompileInvocation();

// };

// class JBBuilder {
// }



// class DriverInputFileEntitiesConsumer final
//     : public BuildingCompilationEntitiesConsumer {
// public:
//   DriverInputFileEntitiesConsumer(Driver &driver);

// public:
//   void CompletedCompilationEntity(const CompilationEntity *entity) override;
// };




