


class TopLevelJobConstructionEntitiesConsumer : public DriverAllocation<JobConstructionEntitiesConsumer>{
protected:
  lvm::SmallVector<const JobConstruction*> constructions;

public:
  virtual void CompletedJobConstruction(const JobConstruction* constructions){}
};

class LinkJobConstructionEntitiesConsumer : public TopLevelJobConstructionEntitiesConsumer {

llvm::SmallVector<const JobConstruction*> constructions;
public:

  void CompletedJobConstruction(const JobConstruction* constructions){}


  void CompletedCompileJobConstruction(llvm::ArrayRef<CompileJobConstruction> inputConstructions){}


  ~LinkJobConstructionEntitiesConsumer(){

    ///AutolinkExtractRequired

    /// Now create the link job 
  }


static LinkJobConstruction* LinkJobConstructionEntitiesConsumer::Create(Driver& driver){
  if(Linking){

  }
  return nullptr;

}


};

class MergeJobConstructionEntitiesConsumer : public TopLevelJobConstructionEntitiesConsumer {

};


class BuildingJobConstructionEntities final {

    MergeModuleJobConstruction *mergeModuleJobConstruction = nullptr;
    LinkJobConstruction *linkJobConstruction = nullptr;

  llvm::SmallVector<LinkJobConstructionEntitiesConsumer> consumer;

void MaybeCreateMergeModuleJobConstruction(){

}
void MaybeCreateLinkJobConstruction(){

}

bool HaslinkJobConstruction(){

}

bool HasMergeModuleJobConstruction(){

}
public:

  void CreateCompileJobConstruction() {
    /// consumers are empty
    if(!IsTopLvelJobConstruction()){
      driver.GetCompilationEntities().AddTopLevelJobConstruction()
    }else{
      // notify consumer 
      CompletedConstruction(compileJobConstruction);
    }
    //GetConsumer()->CompletedCreateCompileJobConstruction(job)
  }
  void CreateObjectJobConstruction(){
    //GetConsumer()->CompletedCreateCompileJobConstruction(job)
  }
  void CompleteCompileJobConstruction(compileJobConstructi){


      ForEachConsumer(){
          consumer->CompletedJobConstruction(compileJobConstructi);
      }
  }
  public:
    /// pass in BuildingJobConstructionEntities and use it as a call back -- may not need to because
    /// each can call ConstructJob 
    BuildingJobConstructionEntities(Driver &driver) : 
      linkJobConstruction(MaybeCreateMergeModuleJobConstruction()), 
        linkJobConstruction(MaybeCreateLinkJobConstruction())
    ~BuildingJobConstructionEntities() {


    }

  public:
    void BuildForNormalCompileStyle(){

        /// for each driver input file 
        switch(GetFileType()){
            case FileType::Stone:
              CreateCompileJobConstruction();
            case FileType::Object:
              CreateObjectJobConstruction();

        }

    }
    void BuildForSingleCompileStyle(){


    }
    void BuildForFlatCompileStyle(){


    }
    // void CreateJobConstruction(DriverInputFile* input){

    //    /// for each driver input file 
    //     switch(input.GetFileType()){
    //         case FileType::Stone:
    //           CreateCompileJobConstruction();
    //         case FileType::Object:
    //           CreateObjectJobConstruction();
    //     }
    // }

  public:
    void BuildForCompileStyle(CompileStyleKind kind){

      switch(kind){
        CompileStyleKind::Normal:
          return BuildForNormalCompileStyle();
      }
    }
  };


  class BuildingJobEntities final {
  public:
    BuildingJobEntities(BuildingJobConstructionEntities& constructions);
    ~BuildingJobEntities();
  };

class BuildingCompilationEntities final {
public:
  BuildingJobEntities jobEntities;
  BuildingJobConstructionEntities jobConstructionEntities;

public:

  BuildingCompilationEntities(){
  }

  ~BuildingCompilationEntities(){}
};

void BuilCompilationEntities(CompilationEntities& entities){

  BuildingCompilationEntities buildingEntities;

  buildingEntities.GetJobCompilationEntities().AddConsumer(LinkJobConstructionEntitiesConsumer::Create());
  buildingEntities.GetJobCompilationEntities().AddConsumer(MergeJobConstructionEntitiesConsumer::Create());

}
