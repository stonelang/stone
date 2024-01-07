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